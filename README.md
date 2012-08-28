journald-cat
============

Tool to take piped JSON and log the fields to the systemd journal.

Example
=======

echo '{"ONE": 2, "TWO": "str5", "MESSAGE": "Hello world!"}' | journald-cat

Authors
=======

David Strauss <david@davidstrauss.net>
Jerry Blakley <jerryblakley@gmail.com>
