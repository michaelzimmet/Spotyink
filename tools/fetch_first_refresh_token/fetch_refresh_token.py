import base64
import hashlib
import http.server
import json
import secrets
import string
import subprocess
import sys
from typing import Any
from urllib import request
from urllib.parse import parse_qs, urlencode, urlparse

AUTH_URL = "https://accounts.spotify.com/authorize"
TOKEN_URL = "https://accounts.spotify.com/api/token"
SCOPE = "user-read-currently-playing"


class AuthorizationHandler(http.server.SimpleHTTPRequestHandler):
    code = None

    def do_GET(self) -> None:
        query = urlparse(self.path).query
        params = parse_qs(query)
        AuthorizationHandler.code = params.get("code", [None])[0]
        self.send_response(200)
        self.end_headers()

    def log_message(self, format: str, *args: Any) -> None:
        # logging in the Terminal is not intended!
        return


def create_random_string(length: int) -> bytes:
    possible_chars = string.ascii_letters + string.digits

    res = ""
    while len(res) < length:
        rand_char = secrets.choice(possible_chars)
        if rand_char not in ["\n", "+", "=", "/"]:
            res += rand_char

    return res.encode("ASCII")


def try_fetch_refresh_token(
    handler: type[AuthorizationHandler],
    client_id: str,
    code_verifier: bytes,
    redirect_uri: str,
) -> str:
    print("Try exchanging code for refresh token...")

    data = urlencode(
        {
            "grant_type": "authorization_code",
            "code": handler.code,
            "redirect_uri": redirect_uri,
            "client_id": client_id,
            "code_verifier": code_verifier,
        }
    ).encode("ascii")

    req = request.Request(
        TOKEN_URL,
        data=data,
        method="POST",
    )

    with request.urlopen(req) as response:
        tokens = json.loads(response.read().decode())

    print("Refresh token: ", tokens["refresh_token"])
    return tokens.get("refresh_token", None)


def main():
    client_id = input("Type in your client_id: ")

    code_verifier = create_random_string(98)
    sha256 = hashlib.sha256(code_verifier).digest()
    code_challenge = base64.urlsafe_b64encode(sha256).decode("ascii").replace("=", "")

    redirect_uri = input("Type in your specified redirect URL: ")
    params = {
        "client_id": client_id,
        "response_type": "code",
        "redirect_uri": redirect_uri,
        "code_challenge_method": "S256",
        "code_challenge": code_challenge,
        "scope": SCOPE,
    }

    url = AUTH_URL + "?" + urlencode(params)

    if not url:
        print("Something went wrong authorization url could not be built.")
        return

    print("Opening Browser for Spotify-Login...")
    subprocess.Popen(
        ["firefox", "--new-window", url],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    print("Wait for redirect with authorization url...")

    auth_handler = AuthorizationHandler
    http_server = http.server.HTTPServer(("127.0.0.1", 8888), auth_handler)
    http_server.handle_request()

    if not auth_handler.code:
        print("No code could be fetched!")
        sys.exit(1)

    print("Received code")
    try_fetch_refresh_token(auth_handler, client_id, code_verifier, redirect_uri)


if __name__ == "__main__":
    main()
