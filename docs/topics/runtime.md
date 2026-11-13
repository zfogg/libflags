# Runtime updates

Runtime reads and writes are instance-based and synchronized. A revision
counter supports optimistic updates, while batch updates validate every value
before committing any of them and advance the revision once.

Callbacks run after the writer lock is released, so application actions cannot
deadlock the context.
