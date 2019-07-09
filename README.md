## Current results

```
WebSocket {url: "wss://127.0.0.1:8443/", readyState: 0, bufferedAmount: 0, onopen: null, onerror: null, …}

index.html:3 WebSocket connection to 'wss://127.0.0.1:8443/' failed: Error in connection establishment: net::ERR_CERT_AUTHORITY_INVALID
```

## How to generate a *FREE* certificate on your own:

For a limited period now you can generate a free certificate on your own, run:

```
$ openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365
```
