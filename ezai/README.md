# ez-ai — AI Standard Library for EZ

Call OpenAI, Anthropic, and Gemini from your EZ scripts with one line.

## Installation

```bash
ez install ai
```

Or manually copy the `ai/` folder into `C:/ezlib/`.

## Quick Start

```ez
use "ai"

# One-liner (auto-detects provider from API key)
response = ask("What is the capital of France?", "sk-YOUR-OPENAI-KEY")
out response
```

## API Reference

### `ask(prompt, apiKey)` → String
Auto-detects provider from key prefix:
- `sk-...` → OpenAI
- `AIza...` → Gemini
- Other → Anthropic

### `ask_with_system(prompt, apiKey, systemPrompt)` → String
Same as `ask()` but with a system prompt.

---

### OpenAI

```ez
response = openai("Hello!", apiKey)
response = openai_model("Hello!", apiKey, "gpt-4o")
response = openai_full("Hello!", apiKey, "gpt-4o", "You are helpful")
```

| Task | Parameters | Default Model |
|------|-----------|---------------|
| `openai(prompt, key)` | prompt, apiKey | gpt-4o-mini |
| `openai_model(prompt, key, model)` | prompt, apiKey, model | — |
| `openai_full(prompt, key, model, system)` | prompt, apiKey, model, systemPrompt | — |

---

### Anthropic

```ez
response = anthropic("Hello!", apiKey)
response = anthropic_model("Hello!", apiKey, "claude-3-haiku-20240307")
response = anthropic_full("Hello!", apiKey, "claude-3-5-sonnet-20241022", "Be concise")
```

| Task | Parameters | Default Model |
|------|-----------|---------------|
| `anthropic(prompt, key)` | prompt, apiKey | claude-3-5-sonnet-20241022 |
| `anthropic_model(prompt, key, model)` | prompt, apiKey, model | — |
| `anthropic_full(prompt, key, model, system)` | prompt, apiKey, model, systemPrompt | — |

---

### Gemini

```ez
response = gemini("Hello!", apiKey)
response = gemini_model("Hello!", apiKey, "gemini-1.5-pro")
response = gemini_full("Hello!", apiKey, "gemini-1.5-flash", "Be brief")
```

| Task | Parameters | Default Model |
|------|-----------|---------------|
| `gemini(prompt, key)` | prompt, apiKey | gemini-1.5-flash |
| `gemini_model(prompt, key, model)` | prompt, apiKey, model | — |
| `gemini_full(prompt, key, model, system)` | prompt, apiKey, model, systemPrompt | — |

---

### Chat Sessions (Multi-Turn)

```ez
use "ai"

# Create a chat
chat = new_chat("openai", apiKey)

# Or with a system prompt
chat = new_chat_with_system("openai", apiKey, "You are a pirate")

# Send messages (history is tracked automatically)
r1 = chat_send(chat, "Hello!")
out r1

r2 = chat_send(chat, "Tell me more")
out r2

# Change model mid-conversation
chat = chat_set_model(chat, "gpt-4o")
```

| Task | Description |
|------|-------------|
| `new_chat(provider, key)` | Create chat session ("openai", "anthropic", "gemini") |
| `new_chat_with_system(provider, key, system)` | Chat with system prompt |
| `chat_send(chat, message)` | Send message, get response |
| `chat_set_model(chat, model)` | Change model |

## Error Handling

All tasks return `nil` on failure and print an error message prefixed with `[ez-ai]`. Wrap calls in `try/catch` for safety:

```ez
try {
    response = ask("Hello", apiKey)
    when response != nil {
        out response
    }
} catch e {
    out "Something went wrong: " + str(e)
}
```

## License

MIT
