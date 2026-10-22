# Protocols and compatibility

## Hysteria2 support

Hysteria2 (`type: hysteria2` / `hy2`) doesn't use TCP — it runs QUIC over UDP, so it is
an entirely different protocol stack from the VLESS family. The implementation lives in
`src/core/src/common/Hysteria2Client.cpp`:

| Layer | Implementation |
|----|------|
| QUIC | OpenSSL 3.2+'s built-in QUIC client API (`OSSL_QUIC_client_method` + `BIO_new_dgram`), no third-party QUIC library |
| TLS | QUIC carries TLS 1.3 internally; ALPN pinned to `h3`; both `skip-cert-verify` and `fingerprint` (certificate SHA-256 pinning) are supported |
| Auth | A minimal HTTP/3 subset: control-stream SETTINGS + `POST https://hysteria/auth`; QPACK uses the static table only (the dynamic table is declared with capacity 0), and server Huffman strings are decoded per RFC 7541 |
| Tunneling | One bidirectional stream per target connection: `0x401`(varint) \| address length \| `host:port` \| padding length \| padding |
| Port hopping | `ports: 60000-63000` style; a random port is picked for each connection |
| Retry | Network-class failures (handshake/auth timeouts) retry on up to 3 other ports within the overall timeout budget; deterministic failures such as a rejected auth are not retried |

**Explicit boundaries** (no silent degradation):

- **No UDP relay**: OpenSSL 3.2 doesn't implement QUIC DATAGRAM frames, so UDP sessions on
  a Hysteria2 node fail loudly and get an ICMP reply (nudging apps back to TCP) instead of
  degrading to a direct connection. UDP relay itself is implemented in the TUN data plane
  (both DIRECT and VLESS can carry it); it is only unavailable when a Hysteria2 node is
  selected.
- **No Salamander obfuscation (`obfs`) and no realm**: selecting them reports a clear
  reason.
- **No stream multiplexing**: every inbound connection establishes its own QUIC session
  (so a new connection costs one extra handshake round-trip, on the order of tens of
  milliseconds). This keeps each connection's objects on its own worker thread with
  nothing shared across threads. If first-byte latency needs work later, per-node session
  sharing plus dedicated I/O threads can be added without touching the protocol layer.

## Compatibility verified against a real subscription

A subscription with 71 nodes, 9 proxy groups and roughly 2000 rules was used for
end-to-end verification; four classes of real-world compatibility problems surfaced and
were fixed:

| Problem | Symptom | Fix |
|------|------|------|
| Subscription User-Agent | Airports identify clients by UA; a non-clash UA gets a trimmed config plus "your proxy software isn't supported" | Default UA is `clash-qt/v<version> (clash.meta)`, the form mainstream clash clients use; overridable in settings |
| Group `type: select` | The parser table only knew `selector`, so only 4 of 9 groups were built | `parseGroupType` now accepts `select` / `urltest` / `loadbalance` / `smart` |
| Built-in policies in a member list | `proxies: [REJECT, DIRECT]` made find() come up empty and the whole group was skipped | DIRECT / REJECT / REJECT-DROP / PASS are registered in the name table |
| Logical rule sub-rules | `AND,((DST-PORT,443),(NETWORK,UDP)),REJECT` — the sub-rules have no target policy and were rejected as having too few fields | Sub-rules get a placeholder target and inherit the outer policy |

Conclusion: the kernel loads every node and group correctly; traffic matching a DIRECT
rule is forwarded normally; VLESS group traffic reaches the internet (verified on real
REALITY + Vision and VLESS + TLS + WebSocket nodes); Hysteria2 nodes work as well (see
above); unimplemented protocols fail loudly rather than silently going direct.
