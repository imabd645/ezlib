# oauth

OAuth 2.0 client for EZ. The authorization code flow with PKCE, refresh,
client credentials, and presets for the providers people actually sign in with.

```ez
use "oauth"

client = OAuth2.provider("github", CLIENT_ID, CLIENT_SECRET,
                         "https://example.com/callback")

# 1. send the user away
start = client.begin()
session["oauth"] = start["session"]            # keep the state and verifier
redirect(start["url"])

# 2. they come back
token = client.complete(query, session["oauth"])
user = client.profile(token)
```

That is the whole login. `begin` and `complete` do the two checks people leave
out — `state` against CSRF, and PKCE against a stolen authorization code.

## Install

```
ez install oauth
```

## Providers

`google` `github` `microsoft` `discord` `slack` `gitlab` `spotify` `linkedin`
`facebook` `twitch`

Each preset carries the endpoints, the default scopes, and the quirk that
otherwise costs an afternoon:

- **GitHub** returns form-encoded token responses unless you send
  `Accept: application/json`, and its API refuses a request with no `User-Agent`.
- **Google** issues a refresh token only with `access_type=offline`, and only
  on the first consent unless you also force `prompt=consent`.
- **Spotify** rejects the client secret in the body and wants HTTP Basic.
- **Twitch** wraps the user in a `data` array.

For a tenant-specific Microsoft app:

```ez
client = OAuth2("id", "secret", redirectUri).applyPreset(microsoftTenant("contoso.com"))
```

Any other provider:

```ez
client = OAuth2(CLIENT_ID, CLIENT_SECRET, "https://example.com/callback")
    .endpoints("https://auth.example.com/authorize", "https://auth.example.com/token")
    .withScopes(["read", "write"])
    .withUserInfo("https://api.example.com/me")
```

## PKCE

On by default, S256.

The authorization code comes back through the browser, so anything that can
observe the redirect — a malicious app registered for the same custom scheme, a
logged URL, a proxy — can steal it and redeem it. With PKCE the code is useless
without the verifier, which never leaves your server. OAuth 2.1 requires it for
every client, not just mobile ones.

`begin()` generates the verifier and puts it in the session it returns. Keep
that session — without it `complete` can neither check the state nor prove the
exchange.

Turn it off only against a server that rejects the extra parameters outright:

```ez
client.withoutPkce()
```

## The token

```ez
token.accessToken
token.header()                                 # "Bearer ya29.a0Af…"
token.authHeaders()                            # { "Authorization": … }
token.isExpired()
token.secondsRemaining()
token.scopes()
token.hasScope("read:user")
```

`expires_in` arrives as a duration and is only meaningful at the moment of
receipt, so it is converted to an absolute time once. Expiry is judged 60
seconds early, so a token that is valid when you check it does not expire
mid-request.

Printing a token shows six characters and its lifetime. An access token in a
log is a live credential.

### Storing it

```ez
session["token"] = token.toJson()
token = Token.fromJson(session["token"])
```

## Refreshing

```ez
token = client.ensureFresh(token)              # refreshes only if it needs it
```

That is the call to make before using a stored token. It returns the token
untouched if it is still good, and untouched again if it has expired but has no
refresh token — the user has to sign in again, and you will find out from the
API call.

`refresh(token)` forces it. The old refresh token is carried over when the
provider does not return a new one, which most do not — dropping it there would
break every later refresh.

## Machine to machine

No user, no browser:

```ez
token = client.clientCredentials(["api.read"])
```

There is no refresh token in this flow. Ask for another when it expires.

## The user

```ez
raw = client.userInfo(token)                   # whatever the provider returns
who = client.profile(token)                    # { id, email, name, raw }
```

`profile` normalises the three fields every provider spells differently —
Google's `sub`, GitHub's `id`, Microsoft's `mail`, Spotify's `display_name` —
so a login does not have to special-case each one. The untouched response is
still there under `raw`.

## Errors

Failures are thrown with the reason and, for the three that everyone hits,
what actually causes them:

```
oauth: invalid_grant (the code or refresh token was already used, has expired,
or the redirect_uri does not match) -- Bad verification code
```

A provider that refuses in the redirect rather than at the token endpoint —
the user pressing "cancel" — surfaces the same way from `complete`.

## Swapping the HTTP layer

```ez
client.useTransport(myPost, myGet)
```

`myPost(url, body, headers)` and `myGet(url, headers)` return the response body.
Useful for retries and timeouts, and it is how the test suite runs the whole
flow without a network.

Note that EZ's HTTP builtins expose the body but not the status code, so
failures are recognised from the error field RFC 6749 §5.2 requires, and a body
that is neither a token nor an error is reported verbatim.

## API

**`OAuth2(clientId, clientSecret, redirectUri)`**
**`OAuth2.provider(name, clientId, clientSecret, redirectUri)`**

| method                        | does                                          |
|-------------------------------|-----------------------------------------------|
| `begin()`                     | `{ url, session }` — send the user to `url`    |
| `beginWith(params)`           | the same, plus extra authorize parameters      |
| `complete(query, session)`    | check the callback and exchange the code       |
| `exchange(code, verifier)`    | redeem a code directly                         |
| `refresh(token)`              | a new token from a refresh token               |
| `ensureFresh(token)`          | refresh only if needed                         |
| `clientCredentials(scopes)`   | machine-to-machine token                       |
| `userInfo(token)`             | the provider's raw user object                 |
| `profile(token)`              | `{ id, email, name, raw }`                     |
| `revoke(token)`               | `true` if the provider supports revocation     |
| `authorizationUrl(params)`    | build the URL yourself                         |

Configuration, all chainable: `endpoints`, `withScopes`, `withUserInfo`,
`withoutPkce`, `useBasicAuth`, `useTransport`, `applyPreset`.

Also exported: `Token`, `createVerifier`, `challengeFor`, `createPair`,
`createState`, `statesMatch`, `encodeForm`, `decodeForm`, `parseBody`,
`PROVIDERS`, `providerNames`, `providerFor`, `microsoftTenant`.

## Tests

```
ez test.ez
```

165 assertions, no network. Includes the RFC 7636 PKCE test vector, and checks
that the verifier never reaches the authorization URL and the client secret
never reaches the browser.

## License

MIT
