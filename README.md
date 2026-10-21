# CLIProxyAPI Daybreak Blue plugin

This ABI v1 plugin adds `gpt-daybreak-blue-latest` to the built-in `codex`
provider without modifying CLIProxyAPI. It can be mounted into the current
stock container, so CLIProxyAPI updates do not require rebasing a source fork.

## Build

```bash
make test
```

The plugin is written in C and produces `build/daybreak-blue.so` for the host
architecture.

## Configure CLIProxyAPI

Mount the shared library into the configured plugin directory and enable it:

```yaml
plugins:
  enabled: true
  dir: "/CLIProxyAPI/plugins"
  configs:
    daybreak-blue:
      enabled: true
      priority: 1
```

Example Compose fragment:

```yaml
services:
  cliproxyapi:
    image: eceasy/cli-proxy-api:latest
    volumes:
      - ./config.yaml:/CLIProxyAPI/config.yaml
      - ./plugins/daybreak-blue.so:/CLIProxyAPI/plugins/daybreak-blue.so:ro
```

The model is appended to every Codex credential. The OpenAI backend still
enforces account entitlement, so a credential without Daybreak access will
receive the upstream error when it tries to use the model.

CLIProxyAPI currently synthesizes Codex client metadata for plugin models from
its default template. The model routes correctly, but its catalog entry does
not include Daybreak's exact `cyber` specialty or service-tier fields.
