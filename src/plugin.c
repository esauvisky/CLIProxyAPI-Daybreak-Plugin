#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define CLIPROXY_EXPORT __declspec(dllexport)
#else
#define CLIPROXY_EXPORT __attribute__((visibility("default")))
#endif

#define ABI_VERSION 1

typedef struct {
	void *ptr;
	size_t len;
} cliproxy_buffer;

typedef int (*cliproxy_host_call_fn)(void *, const char *, const uint8_t *, size_t,
										cliproxy_buffer *);
typedef void (*cliproxy_host_free_fn)(void *, size_t);

typedef struct {
	uint32_t abi_version;
	void *host_ctx;
	cliproxy_host_call_fn call;
	cliproxy_host_free_fn free_buffer;
} cliproxy_host_api;

typedef int (*cliproxy_plugin_call_fn)(const char *, const uint8_t *, size_t,
									  cliproxy_buffer *);
typedef void (*cliproxy_plugin_free_fn)(void *, size_t);
typedef void (*cliproxy_plugin_shutdown_fn)(void);

typedef struct {
	uint32_t abi_version;
	cliproxy_plugin_call_fn call;
	cliproxy_plugin_free_fn free_buffer;
	cliproxy_plugin_shutdown_fn shutdown;
} cliproxy_plugin_api;

static const char registration_response[] =
	"{\"ok\":true,\"result\":{\"schema_version\":1,\"metadata\":{"
	"\"Name\":\"Daybreak Models\",\"Version\":\"0.2.3\","
	"\"Author\":\"esauvisky\","
	"\"GitHubRepository\":\"https://github.com/esauvisky/CLIProxyAPI-Daybreak-Plugin\","
	"\"Logo\":\"https://raw.githubusercontent.com/esauvisky/CLIProxyAPI-Daybreak-Plugin/main/assets/logo.png\","
	"\"ConfigFields\":[]},\"capabilities\":{\"model_registrar\":true}}}";

static const char models_response[] =
	"{\"ok\":true,\"result\":{\"Provider\":\"codex\",\"Models\":[{"
	"\"ID\":\"gpt-daybreak-blue-latest\",\"Object\":\"model\","
	"\"Created\":1783616400,\"OwnedBy\":\"openai\",\"Type\":\"openai\","
	"\"DisplayName\":\"Daybreak Blue\",\"Name\":\"gpt-daybreak-blue-latest\","
	"\"Version\":\"gpt-daybreak-blue-latest\","
	"\"Description\":\"Latest frontier agentic coding model for broad defensive cybersecurity work.\","
	"\"ContextLength\":372000,\"MaxCompletionTokens\":128000,"
	"\"SupportedParameters\":[\"tools\"],"
	"\"SupportedInputModalities\":[\"text\",\"image\"],"
	"\"SupportedOutputModalities\":[\"text\"],"
	"\"Thinking\":{\"Levels\":[\"low\",\"medium\",\"high\",\"xhigh\",\"max\"]},"
	"\"UserDefined\":true},{"
	"\"ID\":\"gpt-daybreak-red-latest\",\"Object\":\"model\","
	"\"Created\":1783616400,\"OwnedBy\":\"openai\",\"Type\":\"openai\","
	"\"DisplayName\":\"Daybreak Red\",\"Name\":\"gpt-daybreak-red-latest\","
	"\"Version\":\"gpt-daybreak-red-latest\","
	"\"Description\":\"Latest frontier agentic coding model for broad defensive cybersecurity work.\","
	"\"ContextLength\":372000,\"MaxCompletionTokens\":128000,"
	"\"SupportedParameters\":[\"tools\"],"
	"\"SupportedInputModalities\":[\"text\",\"image\"],"
	"\"SupportedOutputModalities\":[\"text\"],"
	"\"Thinking\":{\"Levels\":[\"low\",\"medium\",\"high\",\"xhigh\",\"max\"]},"
	"\"UserDefined\":true}]}}";

static void write_response(cliproxy_buffer *response, const char *text) {
	if (response == NULL || text == NULL) {
		return;
	}
	response->len = strlen(text);
	response->ptr = malloc(response->len);
	if (response->ptr == NULL) {
		response->len = 0;
		return;
	}
	memcpy(response->ptr, text, response->len);
}

static int plugin_call(const char *method, const uint8_t *request,
					   size_t request_len, cliproxy_buffer *response) {
	(void)request;
	(void)request_len;
	if (response != NULL) {
		response->ptr = NULL;
		response->len = 0;
	}
	if (method == NULL) {
		write_response(response,
					   "{\"ok\":false,\"error\":{\"code\":\"invalid_method\","
					   "\"message\":\"method is required\"}}");
		return 1;
	}
	if (strcmp(method, "plugin.register") == 0 ||
		strcmp(method, "plugin.reconfigure") == 0) {
		write_response(response, registration_response);
		return 0;
	}
	if (strcmp(method, "model.register") == 0) {
		write_response(response, models_response);
		return 0;
	}
	write_response(response,
				   "{\"ok\":false,\"error\":{\"code\":\"unknown_method\","
				   "\"message\":\"unknown method\"}}");
	return 0;
}

static void plugin_free(void *ptr, size_t len) {
	(void)len;
	free(ptr);
}

static void plugin_shutdown(void) {}

CLIPROXY_EXPORT int cliproxy_plugin_init(const cliproxy_host_api *host,
										 cliproxy_plugin_api *plugin) {
	(void)host;
	if (plugin == NULL) {
		return 1;
	}
	plugin->abi_version = ABI_VERSION;
	plugin->call = plugin_call;
	plugin->free_buffer = plugin_free;
	plugin->shutdown = plugin_shutdown;
	return 0;
}
