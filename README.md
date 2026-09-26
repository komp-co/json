# json

JSON for [KFlat](https://github.com/komp-co/komp): a reader that walks a
document in order and a writer that builds one, compact or indented. There is no
value tree; you read and write fields directly, which keeps it allocation-light.

```toml
[dependencies]
json = "0.2"
```

## Writing

```kf
import json.writer.JsonWriter

var w = JsonWriter.pretty(2)       // JsonWriter.new() writes one line
w.begin_object()
w.field_string("name", "komp")
w.field_int64("stars", 42)
w.key("tags")
w.begin_array()
w.write_string("compiler")
w.write_string("self-hosted")
w.end_array()
w.end_object()
val text = w.finish()
```

```json
{
  "name": "komp",
  "stars": 42,
  "tags": [
    "compiler",
    "self-hosted"
  ]
}
```

The writer places every comma, line break and indent. An object member is a
`key` and one value; `field_*` does both. Values:

| Method | Writes |
|---|---|
| `write_string(str)` | a JSON string, escaped |
| `write_int64` / `write_uint64` | an integer |
| `write_float64` | the shortest text that reads back as the same float; `null` for NaN and the infinities, which JSON cannot spell |
| `write_bool` / `write_null` | `true`, `false`, `null` |
| `write_raw(str)` | already-serialized JSON, as one value |
| `begin_object` … `end_object`, `begin_array` … `end_array` | a nested container |

`is_complete()` says whether every container begun has been ended.
`"text".json_quoted()` gives one string as a JSON literal.

## Reading

```kf
import json.reader.*

var r = JsonReader.new(text)
var name = String.from("")
if r.begin_object() {
    while r.object_has_next() {
        val key = r.read_key()
        when key.as_str() {
            "name" => name = r.read_string()
            _ => r.skip_value()
        }
    }
}
if r.has_error() { println("bad JSON at byte ${r.error_offset()}") }
```

| Method | Reads |
|---|---|
| `begin_object` / `object_has_next` / `read_key` | an object, member by member |
| `begin_array` / `array_has_next` | an array, element by element |
| `read_string` | a string, every escape decoded to UTF-8, surrogate pairs included |
| `read_int64` / `read_uint64` | an integer; a fraction, an exponent or overflow is an error |
| `read_float64` | any number, to the nearest float64 |
| `read_bool` / `read_null` / `take_null` | `true`/`false`, `null`, or `null` if it is next |
| `skip_value` | whatever comes next, nested containers included |
| `peek_kind` | what comes next, as a `JsonKind`, without reading it |
| `at_end` | only whitespace is left |

A read that fails returns a default and sets the error: `has_error()` and
`error_offset()`, the byte where it happened. Every read after the first error
fails too, so checking once at the end is enough. The reader is strict JSON:
no trailing commas, no leading zeros, no raw control bytes in strings.

## Developing

```sh
komp test .
```

`json` needs a C compiler, as every komp build does: `read_float64` parses
through the C library's `strtod`.

## License

[MIT](LICENSE)
