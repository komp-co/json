# json

A small JSON reader/writer library for [**KFlat / komp**](https://github.com/komp-co/komp),
written in `.kf`. It provides a pull-based parser and a streaming serializer with
no dynamic JSON value tree — you read and write fields directly, which keeps it
allocation-light and a natural fit for komp's move/ownership model.

It is used by the komp compiler itself (the `kf-driver` and `kf-interface`
crates) to read and write crate-interface metadata.

## Features

- **Pull parser** (`JsonReader`) — walk objects and arrays field-by-field; reads
  strings (with `\`-escapes and `\uXXXX`), integers, booleans, and `null`.
- **Streaming writer** (`JsonWriter`, `JsonArrayWriter`) — append fields/items
  and `finish()` to a `String`; keys and string values are escaped via
  `json_quote`.
- No JSON value/DOM type, no reflection — direct read/write only.

## Usage

Add it as a path dependency in your `kf.toml`:

```toml
[dependencies]
json = { path = "../json" }
```

### Writing

```kf
import json.writer.*

var w = new_json_writer()
w.field_string("name", "komp")
w.field_int64("count", 42)
w.field_bool("ok", true)
val out = w.finish()          // {"name":"komp","count":42,"ok":true}
```

Arrays use `JsonArrayWriter` (`item_string` / `item_raw` / `finish`), and
`field_raw` / `item_raw` splice already-serialized JSON in verbatim.

### Reading

```kf
import json.reader.*

var r = new_json_reader("{\"name\":\"komp\",\"count\":42}")
if r.begin_object() {
    while r.object_has_next() {
        val key = r.read_key()
        if key.as_str() == "name"  { val name  = r.read_string() }
        if key.as_str() == "count" { val count = r.read_int64()  }
    }
}
// check r.has_error() after parsing
```

## Public API

**`src/reader.kf`** — `JsonReader`:
`new_json_reader(source)`, `has_error()`, `begin_object()`, `object_has_next()`,
`read_key()`, `begin_array()`, `array_has_next()`, `read_string()`,
`read_int64()`, `read_bool()`, `read_null()`.

**`src/writer.kf`** — `JsonWriter`:
`new_json_writer()`, `field_string/bool/null/int64/uint32/raw(key, …)`,
`finish()`; `JsonArrayWriter`: `new_json_array_writer()`,
`item_string/item_raw`, `finish()`; plus the standalone `json_quote(str)`.

## Requirements

Depends on komp's standard `core` and `alloc` crates. The `kf.toml` resolves
them at `../komp/libs/{core,alloc}`, so keep a komp checkout as a sibling
directory named lowercase `komp`:

```
.../
├── komp/     # the compiler + libs/core, libs/alloc
└── json/     # this repo
```

Build/test with the komp compiler:

```sh
komp test .        # runs the reader/writer test suites
```

## License

[MIT](LICENSE) — free to use, modify, and distribute.
