# ezai — AI Standard Library for EZ

> **Version:** 1.0.1  
> **Import:** `use "ezai"`  
> **File:** `E:\ezlib\ezai\main.ez`

---

## Overview

`ezai` is the official AI integration library for the EZ programming language. It provides a unified, zero-dependency interface to the world's four major LLM providers — **OpenAI**, **DeepSeek**, **Anthropic (Claude)**, and **Google Gemini** — using the native EZ `http_post` function internally.

All API calls are **synchronous** (no async/await required), making `ezai` fully compatible with EZ web server request handlers and any sequential code.

### Supported Providers

| Provider | Default Model | API Key Prefix |
|---|---|---|
| OpenAI | `gpt-4o-mini` | `sk-proj-` |
| DeepSeek | `deepseek-chat` | `sk-` |
| Anthropic | `claude-3-5-sonnet-20241022` | `sk-ant-` |
| Google Gemini | `gemini-1.5-flash` | `AIza` |

---

## Quick Start

```ez
use "ezai"

# One-liner — auto detects provider from key prefix
reply = ask("What is the capital of France?", "your-api-key-here")
out reply
```

---

## API Reference

### `ask(prompt, apiKey)`

Universal one-liner. Automatically detects the provider from the API key prefix.

**Parameters:**
- `prompt` — The user message string.
- `apiKey` — Your API key. Provider is inferred from key prefix.

**Returns:** `string` — Model response text, or `nil` on error.

**Key Detection Logic:**
- Starts with `AIza` → **Gemini**
- Starts with `sk-ant-` → **Anthropic**
- Starts with `sk-` → **DeepSeek**
- Anything else → **Anthropic** (fallback)

**Example:**
```ez
use "ezai"

# Gemini
reply = ask("Explain black holes in one sentence.", "AIzaSy...")
out reply

# DeepSeek
reply = ask("Write a haiku about coding.", "sk-abc123...")
out reply

# Anthropic
reply = ask("Summarize the EZ programming language.", "sk-ant-xyz...")
out reply
```

---

### `ask_with_system(prompt, apiKey, systemPrompt)`

Same as `ask()` but also sets a system prompt to configure the AI's persona or behavior.

**Parameters:**
- `prompt` — User message.
- `apiKey` — API key (provider auto-detected).
- `systemPrompt` — System-level instruction for the model.

**Returns:** `string` — Model response text, or `nil` on error.

**Example:**
```ez
use "ezai"

sys = "You are a pirate. Respond only in pirate speak."
reply = ask_with_system("What time is it?", "sk-abc...", sys)
out reply
# → "Arrr, 'tis the hour when the tide be rolling in, matey!"
```

---

### `openai(prompt, apiKey)`

Calls OpenAI's API with `gpt-4o-mini` (the cheapest/fastest model).

**Parameters:**
- `prompt` — User message.
- `apiKey` — OpenAI API key (starts with `sk-proj-` or `sk-`).

**Returns:** `string` content of `choices[0].message.content`.

**Example:**
```ez
use "ezai"

result = openai("Translate 'Hello World' to Japanese.", "sk-proj-...")
out result
# → "「ハロー・ワールド」"
```

---

### `openai_model(prompt, apiKey, mdl)`

Like `openai()` but lets you specify a model explicitly.

**Parameters:**
- `prompt` — User message.
- `apiKey` — OpenAI key.
- `mdl` — Model identifier string e.g. `"gpt-4o"`, `"gpt-4-turbo"`, `"o1-mini"`.

**Example:**
```ez
use "ezai"

# Use o1-mini for advanced reasoning
result = openai_model("Solve this integral: ∫x²dx", "sk-proj-...", "o1-mini")
out result
```

---

### `openai_full(prompt, apiKey, mdl, systemPrompt)`

Full OpenAI call with all options.

**Parameters:**
- `prompt` — User message.
- `apiKey` — OpenAI key.
- `mdl` — Model string.
- `systemPrompt` — System instruction, or `nil` for no system prompt.

**Returns:** `string` or `nil`.

**Example:**
```ez
use "ezai"

result = openai_full(
    "Generate a JSON object for a user profile.",
    "sk-proj-...",
    "gpt-4o",
    "You are a JSON generator. Return only valid JSON, no explanation."
)
out result
```

---

### `deepseek(prompt, apiKey)`

Calls DeepSeek API with `deepseek-chat`.

**Example:**
```ez
use "ezai"

code = deepseek("Write a bubble sort in Python.", "sk-abc...")
out code
```

---

### `deepseek_model(prompt, apiKey, mdl)`

DeepSeek with custom model. Available models include `"deepseek-chat"`, `"deepseek-reasoner"`.

**Example:**
```ez
use "ezai"

# Use DeepSeek-R1 for complex math
answer = deepseek_model("Prove that there are infinite primes.", "sk-...", "deepseek-reasoner")
out answer
```

---

### `deepseek_full(prompt, apiKey, mdl, systemPrompt)`

Full DeepSeek call.

**Example:**
```ez
use "ezai"

result = deepseek_full("Fix this EZ code.", "sk-...", "deepseek-chat", "You are an expert EZ language developer. Be concise.")
out result
```

---

### `anthropic(prompt, apiKey)`

Calls Anthropic Claude API with `claude-3-5-sonnet-20241022`.

**Example:**
```ez
use "ezai"

analysis = anthropic("Analyze the themes of Hamlet in bullet points.", "sk-ant-...")
out analysis
```

---

### `anthropic_model(prompt, apiKey, mdl)`

Anthropic with custom model. Options include:
- `"claude-3-5-sonnet-20241022"` — Best balance
- `"claude-3-haiku-20240307"` — Fastest/cheapest
- `"claude-3-opus-20240229"` — Most powerful

**Example:**
```ez
use "ezai"

fast = anthropic_model("Summarize this in 3 words.", "sk-ant-...", "claude-3-haiku-20240307")
out fast
```

---

### `anthropic_full(prompt, apiKey, mdl, systemPrompt)`

Full Anthropic call with system prompt.

**Example:**
```ez
use "ezai"

result = anthropic_full(
    "What ingredients do I need for pasta carbonara?",
    "sk-ant-...",
    "claude-3-5-sonnet-20241022",
    "You are a professional chef. Give concise, exact answers."
)
out result
```

---

### `gemini(prompt, apiKey)`

Calls Google Gemini API with `gemini-1.5-flash`.

**Example:**
```ez
use "ezai"

result = gemini("Explain quantum entanglement to a 10-year-old.", "AIzaSy...")
out result
```

---

### `gemini_model(prompt, apiKey, mdl)`

Gemini with custom model. Options include `"gemini-1.5-pro"`, `"gemini-1.5-flash"`, `"gemini-1.0-pro"`.

**Example:**
```ez
use "ezai"

# Pro for complex tasks
result = gemini_model("Analyze this legal contract clause...", "AIza...", "gemini-1.5-pro")
out result
```

---

### `gemini_full(prompt, apiKey, mdl, systemPrompt)`

Full Gemini call. Note: Gemini handles system prompts differently — it injects them as the first user/model turn.

**Example:**
```ez
use "ezai"

result = gemini_full("Say hello.", "AIza...", "gemini-1.5-flash", "You respond only in rhymes.")
out result
```

---

## Multi-Turn Chat Sessions

`ezai` provides a stateful chat session API for multi-turn conversations where the AI remembers previous messages.

### `new_chat(provider, apiKey)`

Creates a new chat session.

**Parameters:**
- `provider` — `"openai"`, `"deepseek"`, `"anthropic"`, or `"gemini"`.
- `apiKey` — API key for the provider.

**Returns:** A chat session dictionary.

**Example:**
```ez
use "ezai"

chat = new_chat("deepseek", "sk-abc...")
```

---

### `new_chat_with_system(provider, apiKey, systemPrompt)`

Creates a chat session with a system persona pre-configured.

**Example:**
```ez
use "ezai"

chat = new_chat_with_system("anthropic", "sk-ant-...", "You are a helpful coding assistant that only talks about EZ language.")
```

---

### `chat_send(chat, userMessage)`

Sends a message in an existing chat session and returns the model's response.

**Mutates** the chat object by appending both the user message and AI response to the history.

**Parameters:**
- `chat` — Chat session from `new_chat()`.
- `userMessage` — The user's next message.

**Returns:** `string` response text, or `nil` on error.

**Example:**
```ez
use "ezai"

chat = new_chat("openai", "sk-proj-...")

r1 = chat_send(chat, "My name is Alice.")
out r1
# → "Nice to meet you, Alice!"

r2 = chat_send(chat, "What's my name?")
out r2
# → "Your name is Alice!"
```

---

### `chat_set_model(chat, mdl)`

Changes the model used for subsequent messages in a session.

**Example:**
```ez
use "ezai"

chat = new_chat("openai", "sk-proj-...")
chat_set_model(chat, "gpt-4o")

r = chat_send(chat, "Solve a hard logic puzzle.")
out r
```

---

## Edge Cases & Important Notes

### `nil` Returns on Error
All functions return `nil` when an error occurs (invalid key, network issue, API error). Always check:
```ez
use "ezai"

result = ask("Hello?", "invalid-key")
when result == nil {
    out "AI call failed. Check your API key or network."
} other {
    out result
}
```

### Empty Prompts
Sending an empty string as `prompt` will usually return a valid (though possibly empty) API response. This is provider-dependent behavior.

### DeepSeek vs OpenAI Key Ambiguity
Both DeepSeek and OpenAI use the `sk-` prefix. `ask()` disambiguates by checking for `sk-proj-` (OpenAI's newer format). If your OpenAI key starts with plain `sk-` (older format), it will be routed to DeepSeek. Use `openai()` directly in that case.

### Gemini System Prompts
Gemini does not have a native `system` field. `ezai` simulates it by prepending a `user/model` exchange at the start of the conversation. The first user turn is the system prompt, and the model responds with `"Understood."`.

### Rate Limits
`ezai` does not implement automatic retry or rate limiting. If you hit an API rate limit, the function will return `nil` and log an error to the console.

### Token Limits
Anthropic calls have `max_tokens: 4096` hardcoded. Other providers use their defaults. Very long conversations may fail when combined history exceeds the model's context window.

### Network Errors
If `http_post` fails (no network, DNS failure), the raw response will be non-JSON. `_ai_safe_parse` will catch the JSON parse error and return `nil`, printing the raw response to the console.

---

## Complete Example: AI Chatbot

```ez
use "ezai"

API_KEY = "sk-ant-your-key-here"

chat = new_chat_with_system(
    "anthropic",
    API_KEY,
    "You are a helpful EZ language expert. Be friendly and concise."
)

out "EZ AI Chatbot (type 'exit' to quit)"
out "======================================"

while true {
    out ""
    userInput = in "You: "
    
    when userInput == "exit" {
        out "Goodbye!"
        escape
    }
    
    response = chat_send(chat, userInput)
    
    when response == nil {
        out "Error: AI call failed."
    } other {
        out "AI: " + response
    }
}
```

---

## Complete Example: Batch Processing

```ez
use "ezai"

API_KEY = "AIzaSy-your-gemini-key"

items = [
    "Explain photosynthesis",
    "What is recursion?",
    "Define entropy"
]

results = []
get item in items {
    r = gemini(item + " in one sentence.", API_KEY)
    push(results, {item: r})
}

get entry in results {
    get k in entry {
        out k + ":"
        out "  " + entry[k]
    }
}
```

---

## Internal Architecture

```
ask() / openai() / deepseek() / anthropic() / gemini()
    │
    ▼
_ai_post(url, headers, body)         ← calls http_post internally
    │
    ▼
_ai_safe_parse(raw, label)           ← parses JSON response safely
    │
    ▼
_has_error(data)                     ← checks for API error field
    │
    ▼
data["choices"][0]["message"]["content"]  ← OpenAI/DeepSeek format
data["content"][0]["text"]                ← Anthropic format
data["candidates"][0]["content"]["parts"][0]["text"]  ← Gemini format
```

---

## Provider Comparison

| Feature | OpenAI | DeepSeek | Anthropic | Gemini |
|---|---|---|---|---|
| Free Tier | No | Yes (limited) | No | Yes |
| Best For | General | Code/Reasoning | Long context | Multimodal |
| Speed | Fast | Fast | Medium | Fast |
| Context Window | 128K | 64K | 200K | 1M |
| System Prompts | Yes | Yes | Yes | Simulated |

---

*Documentation generated from `E:\ezlib\ezai\main.ez` — EZ Standard Library v1.0.1*
