# Known differences

- **yaml key order**: yaml-cpp doesn't preserve order when re-emitting maps. Key order in
  a YAML map carries no semantic meaning, so functionality is unaffected; reproducing the
  layout byte for byte would require an order-preserving parser.
- **DoT**: `tls://` DNS servers currently fall back to plaintext TCP, with a note in the
  log. The default DNS config therefore uses DoH.
- **Plaintext HTTP forwarding**: everything is treated as `Connection: close`, with no
  keep-alive reuse.
- **Threading model**: each inbound connection gets a thread plus its own event loop
  (capped at 512); the plan is to move to an event-loop thread pool with async dialing.
- **Traffic accounting**: bytes on the system-proxy path are accumulated by a hook in the
  relay; TUN connections are dialed by the IP stack itself and don't go through
  `handleStream`, so `TunIpStack` reports them via `setTrafficSink()` into
  `Tunnel::addTraffic()`. Both paths feed the same pair of atomic counters, and the UI
  samples them once a second to compute speed.
- **Update check**: the data source is the release list on `api.github.com`; on failure
  (unauthenticated API quota is per egress IP, and shared proxy nodes often get a 403) it
  falls back to `releases.atom`. Both paths skip releases whose tag isn't a version (e.g.
  `continuous_build`) and skip prereleases. Requests leave through the local mixed port,
  so it works with or without the system proxy; it only shows the changelog and a link to
  the release page — downloading and installing is left to the user. A freshly pushed tag
  shows up in the Atom feed immediately (title = tag name, body = tag message) while CI
  is still building and no packages exist yet; such placeholder entries are confirmed
  against the API once and dropped when no release exists, so nobody opens a changelog
  that is just a version number.
- **System proxy on exit**: quitting (closing the window, tray exit, SIGTERM) restores the
  system proxy to direct before stopping the kernel. The test is whether the desktop's
  manual http/https/socks proxies all point at the local inbound port — it only touches
  settings that point at itself, so a corporate proxy you configured is never clobbered.
  A `SIGKILL` or a crash leaves no time for cleanup, and a proxy pointing at an empty port
  stays behind on the desktop.
- **Switch memory**: system-proxy and TUN state are stored separately in `sysProxy.enable`
  and `tun.enable` and restored at startup (once the event loop is running and the window
  is shown): TUN brings the data plane back up, the system proxy points at the local port
  again. A failed restore only shows a message and doesn't change the record. On exit
  **only the desktop proxy settings are reverted, the switch records are left alone** —
  desktop settings are runtime state, the switch records are user intent. Restoring TUN
  when it isn't authorized raises a pkexec password prompt, which is why this doesn't
  happen before the window appears.
- **ICMP (ping)**: with the system proxy, ping doesn't go through the proxy (HTTP/SOCKS
  have no ICMP semantics) — whether it succeeds depends only on direct connectivity from
  the host. In TUN mode, pinging a domain is answered locally by the IP stack on the
  fake-ip (RTT around 0.1 ms, **not an indication of node quality**), while a real IP
  immediately returns host unreachable. Use the latency test on the proxies page to judge
  node latency.
- **Config reload**: `Hub::applyConfig` swaps the config and rebuilds the adapter
  registry, but it does not keep the inbound listeners alive — whoever reloads must
  decide whether to listen again. `AppContext::reloadKernel` therefore records whether the
  kernel was running *before* applying the config and restarts it afterwards; checking
  `isRunning()` after the apply always reads `false` and silently leaves the kernel
  stopped.
