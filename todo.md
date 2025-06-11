TODO: RFC 2812 Compliance

1. Registration
   ✅- Enforce correct registration sequence: PASS before NICK/USER, NICK and USER before any other command.
   ❓(001 está, falta o resto)- Send all required welcome numerics: 001, 002, 003, 004, etc.
   ❓(acho que esta em falta)- Send RPL_YOURHOST, RPL_CREATED, RPL_MYINFO.
   ✅- Send correct error numerics for registration failures (e.g., 462, 464).

~~2. PING/PONG ~~
  ~~ - Implement PING and PONG commands for client-server keepalive.~~

❌ (nao é preciso)
3. MOTD
   - Implement MOTD command and send numerics 375, 372, 376.

❌ (nao é preciso)
4. WHOIS/WHOWAS
   - Implement WHOIS and WHOWAS commands with correct replies.

❓(WHO esta implementado, NAMES e LIST não está feito e não é pedido)
   5. NAMES, LIST, and Other Queries
   - Implement NAMES (353, 366) and LIST (321, 322, 323) commands.

❌ (nao é preciso)
6. User Modes
   - Implement user modes (e.g., +i for invisible, +o for IRC operator).

❌ (nao é preciso)
7. NOTICE Command
   - Implement NOTICE command (must not trigger automatic replies).

✅ (já implementado, falta verificar se estão todos os que são necessários)
8. Numeric Replies
   - Ensure all commands and errors use correct RFC numerics (not just plain text).

✅ (já implementado)
9. Case Mapping
   - Make all nickname and channel comparisons RFC-compliant (case-insensitive, rfc1459 casemapping).

❌ (nao é preciso)
10. Channel/Client Masks and Wildcards
    - Support wildcards in commands like WHO, NAMES, etc.

❌ (nao é preciso/subject nao fala sobre isto)
11. Away Messages
    - Implement AWAY command and away replies (301).

❌ (nao é preciso/subject nao fala sobre isto)
12. Operator Commands
    - Implement KILL, OPER, DIE, RESTART, SQUIT.

❌ (nao é preciso/subject nao fala sobre isto)
13. Server Info and Stats
    - Implement VERSION, INFO, TIME, ADMIN, LUSERS, STATS.

14. Channel Modes
   ✅ (já implementado)
    - Already implemented: +i (invite-only), +t (topic restricted), +k (key), +l (limit), +o (operator).
   ❌ (nao é preciso/subject nao fala sobre isto)
    - Still missing: +b (ban), +e (exception), +I (invite list), +n (no outside messages), +m (moderated), +s (secret), +p (private), +v (voice), etc.
   ❌ (nao é preciso/subject nao fala sobre isto)
    - Support multi-prefix in NAMES replies (@, +).

❌ (nao é preciso/subject nao fala sobre isto)
16. Channel Ban/Exception/Invite Lists
    - Implement ban (+b), exception (+e), and invite (+I) lists.

✅ (verificar, mas já implementado)
17. Proper Handling of ERRORS and Disconnects
    - Ensure all disconnects and errors send correct numerics/messages.

❌ (nao é preciso/subject nao fala sobre isto)
18. JOIN 0
    - Support JOIN 0 to leave all channels.

❌ (nao é preciso/subject nao fala sobre isto)
19. Multi-target PRIVMSG/NOTICE
    - Support sending to multiple targets (comma-separated).

❌ (nao é preciso)
20. Topic Set Time and Setter
    - Send RPL_TOPICWHOTIME (333) on join or topic query.

❌ (nao é preciso/subject nao fala sobre isto)
21. Channel/Client Name Length and Format
    - Enforce all RFC rules for nick/channel names (length, allowed characters).

❌ (nao é preciso)
22. Channel Voice, Secret, Private, Moderated, No Outside Messages
    - Implement +v, +s, +p, +m, +n modes.

✅ (já implementado +l)
23. Channel/Client List Limits
    - Support all related numerics and limits.

✅ (já implementado)
24. Channel Key/Password Handling
    - Handle all edge cases for +k (key) mode.

✅ (já implementado)
25. Replies to MODE Queries
    - Reply with 324 for user modes as well as channel modes.

✅ (já implementado)
26. WHO Reply Details
    - Ensure WHO replies include all required details and flags.

✅ (já implementado)
27. Proper QUIT/PART/KICK/INVITE Notifications
    - Notify all affected users as per RFC.

✅ (já implementado)
28. Channel/Client Quit/Part/Disconnect Edge Cases
    - Handle all edge cases and send all required numerics/messages.

❌ (o subjet diz: "You must not implement server-to-server communication.")
29. Server-to-Server Protocol (?)
    - (Optional for simple server) Implement server-to-server commands.
