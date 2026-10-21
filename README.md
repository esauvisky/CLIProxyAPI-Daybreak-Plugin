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

## Install from the management interface

Add this registry URL to `plugins.store-sources` in the management interface's
configuration editor:

```text
https://raw.githubusercontent.com/esauvisky/CLIProxyAPI-Daybreak-Plugin/main/registry.json
```

The resulting plugin configuration is:

```yaml
plugins:
  enabled: true
  store-sources:
    - "https://raw.githubusercontent.com/esauvisky/CLIProxyAPI-Daybreak-Plugin/main/registry.json"
```

Save the configuration, open **Plugins > Store**, find **Daybreak Blue**, and
click **Install**. The store downloads the correct Linux archive, verifies its
checksum, installs the plugin, and enables its per-plugin configuration. Keep
the plugins directory on a persistent container volume so the installed file
survives container replacement.

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
