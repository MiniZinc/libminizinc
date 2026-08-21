# syntax=docker/dockerfile:1
#
# Four images from one prebuilt CLI bundle, per libc:
#
#   dist          just the MiniZinc tree, on scratch. No libc, nothing to patch,
#                 meant for `COPY --from=` into an image you maintain.
#   runtime-gnu   dist + Google's distroless/cc (glibc, libstdc++, no shell).
#   runtime-musl  dist + the four files musl needs, lifted out of Alpine. Alpine
#                 is only a source of files here; it is not the shipped base.
#
# The build context holds an already-extracted bundle per platform, so nothing is
# compiled here: <TARGETPLATFORM>/{bin,lib,share}.

ARG DISTROLESS=gcr.io/distroless/cc-debian13:latest
ARG DEBIAN=debian:13-slim
ARG ALPINE=alpine:latest

FROM scratch AS dist
ARG TARGETPLATFORM
COPY ${TARGETPLATFORM}/ /opt/minizinc/

FROM ${DEBIAN} AS gnu-source
RUN apt-get update && apt-get install -y --no-install-recommends zlib1g

FROM ${ALPINE} AS musl-source
RUN apk add --no-cache libstdc++ zlib

FROM ${DISTROLESS} AS runtime-gnu
COPY --from=gnu-source /usr/lib/*-linux-gnu/libz.so.1* /opt/minizinc/lib/
COPY --from=dist /opt/minizinc /opt/minizinc
# libhighs is dlopened from the bundle's own lib directory.
ENV PATH=/opt/minizinc/bin:$PATH LD_LIBRARY_PATH=/opt/minizinc/lib
ENTRYPOINT ["/opt/minizinc/bin/minizinc"]

FROM scratch AS runtime-musl
COPY --from=musl-source /lib/ld-musl-*.so.1 /lib/
COPY --from=musl-source /usr/lib/libstdc++.so.6* /usr/lib/libgcc_s.so.1* /usr/lib/libz.so.1 /usr/lib/
COPY --from=dist /opt/minizinc /opt/minizinc
ENV PATH=/opt/minizinc/bin:$PATH LD_LIBRARY_PATH=/opt/minizinc/lib TMPDIR=/
ENTRYPOINT ["/opt/minizinc/bin/minizinc"]
