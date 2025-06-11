TODO: RFC 2812 Compliance

1. Registration
   - Enforce correct registration sequence: PASS before NICK/USER, NICK and USER before any other command.
   - Send all required welcome numerics: 001, 002, 003, 004, etc.
   - Send RPL_YOURHOST, RPL_CREATED, RPL_MYINFO.
   - Send correct error numerics for registration failures (e.g., 462, 464).

~~2. PING/PONG 
   - Implement PING and PONG commands for client-server keepalive.~~

3. MOTD
   - Implement MOTD command and send numerics 375, 372, 376.

4. WHOIS/WHOWAS
   - Implement WHOIS and WHOWAS commands with correct replies.

5. NAMES, LIST, and Other Queries
   - Implement NAMES (353, 366) and LIST (321, 322, 323) commands.

6. User Modes
   - Implement user modes (e.g., +i for invisible, +o for IRC operator).

7. NOTICE Command
   - Implement NOTICE command (must not trigger automatic replies).

8. Numeric Replies
   - Ensure all commands and errors use correct RFC numerics (not just plain text).

9. Case Mapping
   - Make all nickname and channel comparisons RFC-compliant (case-insensitive, rfc1459 casemapping).

10. Channel/Client Masks and Wildcards
    - Support wildcards in commands like WHO, NAMES, etc.

11. Away Messages
    - Implement AWAY command and away replies (301).

12. Operator Commands
    - Implement KILL, OPER, DIE, RESTART, SQUIT.

13. Server Info and Stats
    - Implement VERSION, INFO, TIME, ADMIN, LUSERS, STATS.

14. Channel Modes
    - Already implemented: +i (invite-only), +t (topic restricted), +k (key), +l (limit), +o (operator).
    - Still missing: +b (ban), +e (exception), +I (invite list), +n (no outside messages), +m (moderated), +s (secret), +p (private), +v (voice), etc.
    - Support multi-prefix in NAMES replies (@, +).

15. Channel Ban/Exception/Invite Lists
    - Implement ban (+b), exception (+e), and invite (+I) lists.

16. Proper Handling of ERRORS and Disconnects
    - Ensure all disconnects and errors send correct numerics/messages.

17. JOIN 0
    - Support JOIN 0 to leave all channels.

18. Multi-target PRIVMSG/NOTICE
    - Support sending to multiple targets (comma-separated).

19. Topic Set Time and Setter
    - Send RPL_TOPICWHOTIME (333) on join or topic query.

20. Channel/Client Name Length and Format
    - Enforce all RFC rules for nick/channel names (length, allowed characters).

21. Channel Voice, Secret, Private, Moderated, No Outside Messages
    - Implement +v, +s, +p, +m, +n modes.

22. Channel/Client List Limits
    - Support all related numerics and limits.

23. Channel Key/Password Handling
    - Handle all edge cases for +k (key) mode.

24. Replies to MODE Queries
    - Reply with 324 for user modes as well as channel modes.

25. WHO Reply Details
    - Ensure WHO replies include all required details and flags.

26. Proper QUIT/PART/KICK/INVITE Notifications
    - Notify all affected users as per RFC.

27. Channel/Client Quit/Part/Disconnect Edge Cases
    - Handle all edge cases and send all required numerics/messages.

28. Server-to-Server Protocol (?)
    - (Optional for simple server) Implement server-to-server commands.
