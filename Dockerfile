# TEROS Enterprise Development Environment
# Multi-stage build for kernel development, Lambda³ service, and testing

FROM ubuntu:22.04 AS base

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Rome

# Install base dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    cmake \
    nasm \
    binutils \
    qemu-system-x86 \
    qemu-utils \
    gdb \
    python3.11 \
    python3.11-dev \
    python3-pip \
    git \
    curl \
    wget \
    vim \
    nano \
    less \
    tmux \
    strace \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
COPY requirements.txt /tmp/
RUN pip3 install --upgrade pip && \
    pip3 install -r /tmp/requirements.txt

WORKDIR /teros

# Development stage
FROM base AS development

# Install additional development tools
RUN apt-get update && apt-get install -y \
    cppcheck \
    clang-format \
    clang-tidy \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

# Copy all source code
COPY . /teros/

# Build toolchain
RUN make toolchain || true

# Expose ports for services
EXPOSE 8000 8080 8888

# Default command
CMD ["/bin/bash"]

# Production stage (for compiled kernel)
FROM base AS production

COPY --from=development /teros/bin /teros/bin
COPY --from=development /teros/build /teros/build

CMD ["qemu-system-x86_64", "-kernel", "/teros/bin/teros.bin", "-serial", "stdio"]

