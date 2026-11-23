import fs from "node:fs/promises";
import path from "node:path";

const packageDir = path.resolve(import.meta.dirname, "..");
const source = path.resolve(packageDir, "../../scaffold/presets/exam-io-c99");
const output = path.join(packageDir, "preset");
const manifest = JSON.parse(await fs.readFile(path.join(source, "manifest.json"), "utf8"));
const { version } = JSON.parse(await fs.readFile(path.join(packageDir, "package.json"), "utf8"));
const manualName = "manual.zh-CN.md";
const manual = await fs.readFile(path.join(source, manualName), "utf8");
if (manual.split("@PRESET_VERSION@").length !== 2)
  throw Error("The preset manual must have exactly one version placeholder.");
const tokenNoise = /\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\[\s\S]|[^"\\])*"|'(?:\\[\s\S]|[^'\\])*'/g;
const mask = (code) => code.replace(tokenNoise, (part) => part.replace(/[^\n]/g, " "));

function* declarations(code, file) {
  const masked = mask(code);
  let end = 0;
  while (end < code.length) {
    const tail = masked.slice(end);
    const offset = tail.search(/\S/);
    if (offset < 0) return;
    const start = end + offset;
    const body = masked.slice(start);
    let name;
    let callback = false;
    if (body.startsWith("#")) {
      const guarded = /^#ifndef (\w+)[ \t]*\n#define \1[^\n]*\n#endif[^\n]*(?:\n|$)/.exec(body);
      const define = /^#define (\w+)(?:[^\\\n]|\\[^\n]|\\\n)*(?:\n|$)/.exec(body);
      const directive = guarded ?? define;
      if (!directive) throw Error(`Unsupported directive in ${file}: ${body.split("\n")[0]}`);
      name = directive[1];
      end = start + directive[0].length;
    } else {
      const type = /^typedef\b/.test(body);
      const fn = /^static\s+[\w\s*]+?\b([A-Za-z_]\w*)\s*\(/.exec(body);
      const prototype = /^[\w \t*]+?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*;/.exec(body);
      callback = !type && !!prototype;
      if (!type && !fn && !prototype) throw Error(`Unsupported declaration in ${file}: ${body.split("\n")[0]}`);
      let depth = 0;
      let closed = false;
      for (const token of body.matchAll(/[{};]/g)) {
        if (token[0] === "{") depth++;
        if (token[0] === "}") depth--;
        if (depth === 0 && token[0] === (type || callback ? ";" : "}")) {
          end = start + token.index + 1;
          closed = true;
          break;
        }
      }
      if (!closed) throw Error(`Unclosed declaration in ${file}`);
      name = type ? /\b([A-Za-z_]\w*)\s*;$/.exec(masked.slice(start, end))?.[1] : (prototype ?? fn)[1];
      if (!name) throw Error(`Unsupported typedef in ${file}`);
    }
    yield { name, callback, code: code.slice(start, end).trimEnd() + "\n" };
  }
}

if (manifest.schemaVersion !== 1 || manifest.language !== "c" ||
    manifest.standard !== "c99" || !Array.isArray(manifest.components))
  throw Error("Unsupported exam-io-c99 preset manifest");

const definitions = new Map();
const callbacks = new Set();
for (const component of manifest.components) {
  if (!/^components\/[A-Za-z0-9_/-]+\.h$/.test(component.source) || component.source.includes(".."))
    throw Error("Unsafe component path: " + component.source);
  const input = await fs.readFile(path.join(source, component.source), "utf8");
  for (const entry of declarations(input, component.source)) {
    const existing = definitions.get(entry.name);
    if (existing && existing !== entry.code)
      throw Error("Conflicting definitions of " + entry.name);
    definitions.set(entry.name, entry.code);
    if (entry.callback) callbacks.add(entry.name);
  }
  for (const symbol of component.symbols)
    if (!definitions.has(symbol)) throw Error("Missing definition for " + symbol);
}

const components = [...definitions].map(([id, code]) => ({
  id,
  symbols: callbacks.has(id) ? [] : [id],
  source: `components/${id}.h`,
  requires: [...new Set(mask(code).match(/[A-Za-z_]\w*/g))].filter(
    (name) => name !== id && definitions.has(name)),
}));
const byName = new Map(components.map((component) => [component.id, component]));
const ordered = new Map();
const visiting = new Set();
function visit(id) {
  if (ordered.has(id)) return;
  if (visiting.has(id)) throw Error("Cyclic C preset dependency at " + id);
  visiting.add(id);
  const component = byName.get(id);
  for (const dependency of component.requires) visit(dependency);
  visiting.delete(id);
  ordered.set(id, component);
}
for (const component of components) visit(component.id);

await fs.rm(output, { recursive: true, force: true });
await fs.mkdir(path.join(output, "components"), { recursive: true });
for (const component of ordered.values())
  await fs.writeFile(path.join(output, component.source), definitions.get(component.id));
await fs.writeFile(path.join(output, "manifest.json"),
  JSON.stringify({ ...manifest, components: [...ordered.values()] }, null, 2) + "\n");
await fs.writeFile(path.join(output, manualName), manual.replace("@PRESET_VERSION@", version));
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/CC-BY-4.0.txt"),
  path.join(output, "CC-BY-4.0.txt"));
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/MIT-0.txt"), path.join(packageDir, "LICENSE"));
console.log(`Packed ${components.length} C99 components with individual dependency lists.`);
