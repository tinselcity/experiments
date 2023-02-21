## tee_splice
A SOCKS(v5) proxy toy using Linux [`splice(7)`](https://man7.org/linux/man-pages/man2/splice.2.html) and [`tee(7)`](https://man7.org/linux/man-pages/man1/tee.1.html).

### So what?

if [`SO_ZEROCOPY`](https://www.kernel.org/doc/html/v4.15/networking/msg_zerocopy.html) is enabled, `splice`-ing  could be an efficient way to do pass-thru proxies, and using `tee` one could pick off the data efficiently for local file/content cache'ing.

In practive, however most proxies are using some form of "end-to-end" encryption, where client terminates to the proxy one way (in PKI using the proxy server certificate), and to the upstream server with another (using the upstream server certificate).

#### References:

- CloudFlare blog on attempt to use splice for optimizing away user space copying: [https://blog.cloudflare.com/sockmap-tcp-splicing-of-the-future/](https://blog.cloudflare.com/sockmap-tcp-splicing-of-the-future/)
- Viacheslav Biriukov's [What every SRE should know about GNU/Linux shell related internals](https://biriukov.dev/docs/fd-pipe-session-terminal/2-pipes/#pipe-performance-splice-vmsplice-and-tee)