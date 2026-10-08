# libcsp — VCSP maintenance fork

This is a maintenance fork of [libcsp](https://github.com/libcsp/libcsp).

| | |
|---|---|
| Upstream | https://github.com/libcsp/libcsp |
| Base release | `v2.1` (`48f7fb0b57f610bf65bab1aa2d1357c3b9722782`) |
| Maintenance branch | `v2.1-vinspace` |
| Release tags | `v2.1-vinspace.1`, `v2.1-vinspace.2`, ... |
| Change log | [CHANGELOG-VCSP.md](CHANGELOG-VCSP.md) |
| License | MIT, unchanged (see `LICENSE`) |

## Rules

1. **Prefer**:  Bug fixes, no new features, no API changes, no wire/link-format changes.

2. **Upstream** Commit with `git cherry-pick -x <hash>` so the original hash stays in the message.

3. **One fix per commit.** 
4. **Every change goes through a pull request** into `v2.1-vinspace`, 

5. **Every merged change gets a line in `CHANGELOG-VCSP.md`.**

## Working with the fork

```sh
git clone -b v2.1-vinspace https://github.com/itwasme-ulrich/libcsp.git
cd libcsp
git remote add upstream https://github.com/libcsp/libcsp.git
git remote set-url --push upstream DISABLED   # never push to upstream by mistake
git fetch upstream --tags

# a fix
git switch -c fix/<topic> v2.1-vinspace
git cherry-pick -x <upstream-hash>             # or commit fix
cmake -S . -B build -G Ninja -DCSP_USE_RDP=ON \
      -DCMAKE_C_FLAGS="-Wall -Wextra -Werror -Wno-unused-parameter"
ninja -C build
git push -u origin fix/<topic>                 # PR base: <>/libcsp, v2.1-vinspace
```

**When opening a pull request, check that the base repository is this fork and not
`libcsp/libcsp`**

## Consuming a release

Projects copy the sources that build from a release tag and record that tag next to
the copy.
