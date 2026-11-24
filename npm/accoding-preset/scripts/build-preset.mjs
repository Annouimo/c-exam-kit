import fs from "node:fs/promises";
import path from "node:path";

const packageDir = path.resolve(import.meta.dirname, "..");
const source = path.resolve(packageDir, "../../scaffold/presets/exam-io-c99");
const output = path.join(packageDir, "preset");
const config = JSON.parse(await fs.readFile(path.join(source, "preset.json"), "utf8"));
const { version } = JSON.parse(await fs.readFile(path.join(packageDir, "package.json"), "utf8"));
const tokenNoise = /\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\[\s\S]|[^"\\])*"|'(?:\\[\s\S]|[^'\\])*'/g;
const mask = (code) => code.replace(tokenNoise, (part) => part.replace(/[^\n]/g, " "));

function* declarations(code, file) {
  const masked = mask(code);
  let end = 0;
  while (end < code.length) {
    const gap = end;
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
    const documentation = [...code.slice(gap, start).matchAll(/\/\*\*([\s\S]*?)\*\//g)]
      .map(match => match[1].split("\n").map(line => line.replace(/^\s*\* ?/, "").trimEnd())
        .join("\n").trim());
    const raw = code.slice(start, end);
    const macro = /^#define\s+\w+\([^\n]*?\)/.exec(raw);
    const signature = macro ? macro[0].replace(/^#define\s+/, "") :
      body.startsWith("#") ? name : typeDefinition(body) ? raw.trim() :
      code.slice(start, start + body.search(/[;{]/)).trim().replace(/^static\s+(?:inline\s+)?/, "") + ";";
    yield { name, callback, documentation, signature, code: code.slice(start, end).trimEnd() + "\n" };
  }
}

function typeDefinition(body) {
  return /^typedef\b/.test(body);
}

const componentDirectory = path.join(source, "components");
const files = (await fs.readdir(componentDirectory)).filter(file => file.endsWith(".h")).sort();
const definitions = new Map();
const referenceEntries = [];
for (const file of files) {
  const input = await fs.readFile(path.join(componentDirectory, file), "utf8");
  let category;
  for (const entry of declarations(input, file)) {
    if (definitions.has(entry.name)) throw Error("Duplicate declaration: " + entry.name);
    let required = [], publiclyAvailable = false;
    for (const block of entry.documentation) {
      const lines = block.split("\n");
      let title, members, id = entry.name, keywords = [];
      const details = [], examples = new Map(), variants = [], notes = new Map();
      let section = details;
      for (const line of lines) {
        const [, tag, value = ""] = /^@(\w+)(?: (.*))?$/.exec(line) ?? [];
        switch (tag) {
          case "public": publiclyAvailable = true; break;
          case "requires": required = value.split(/\s+/).filter(Boolean); break;
          case "category": category = value; break;
          case "id": id = value; break;
          case "title": title = value; break;
          case "keywords": keywords = value.split(/\s+/).filter(Boolean); break;
          case "members": members = value.split(/\s+/).filter(Boolean); break;
          case "variant": {
            const [key, ...names] = value.trim().split(/\s+/);
            if (!key || !names.length || variants.some(v => v.key === key)) throw Error("Invalid variant in " + id);
            variants.push({key, names});
            break;
          }
          case "example": {
            // Legacy @example code lines and the new multiline blocks both work.
            const key = variants.some(v => v.key === value) ? value : "";
            if (!examples.has(key)) examples.set(key, []);
            section = examples.get(key);
            if (value && !key) section.push(value);
            break;
          }
          case "note": {
            if (!variants.some(v => v.key === value)) throw Error("Unknown note variant in " + id);
            section = notes.get(value) ?? [];
            notes.set(value, section);
            break;
          }
          case undefined: section.push(line); break;
          default: throw Error("Unknown annotation @" + tag + " in " + file);
        }
      }
      if (title) referenceEntries.push({ id, category, title, keywords,
        members: members ?? (variants.length ? [] : [entry.name]),
        description: details.join("\n").trim(),
        example: (examples.get("") ?? []).join("\n").trimEnd(),
        variants, examples, notes });
    }
    if (!category) throw Error("Missing category in " + file);
    if (publiclyAvailable && entry.callback) throw Error("Callback declaration marked public: " + entry.name);
    definitions.set(entry.name, { id: entry.name, symbols: publiclyAvailable ? [entry.name] : [],
      requires: required, code: entry.code, category, signature: entry.signature });
  }
}

const ordered = new Map();
const visiting = new Set();
function visit(id) {
  if (ordered.has(id)) return;
  if (visiting.has(id)) throw Error("Cyclic C preset dependency at " + id);
  const component = definitions.get(id);
  if (!component) throw Error("Unknown C preset dependency: " + id);
  visiting.add(id);
  for (const dependency of component.requires) visit(dependency);
  visiting.delete(id);
  ordered.set(id, component);
}
for (const id of definitions.keys()) visit(id);

const layout = JSON.parse(await fs.readFile(path.join(source, "reference-layout.json"), "utf8"));
const strict = new Set(layout.strictCategories);
const covered = new Set();
const documents = new Map();
const categories = ["输入和输出", "数组", "字符串", "数学、数位与二分", "大整数、枚举及其他类型"];
const typeNames = { int: "int", ll: "long long", double: "double" };
function signature(name, id) {
  const declaration = definitions.get(name);
  if (!declaration?.symbols.length) throw Error("Unknown public function in " + id + ": " + name);
  covered.add(name);
  return declaration.signature;
}
function expand(template, variant, id) {
  const values = { type: typeNames[variant.key] ?? variant.key,
    ...Object.fromEntries(variant.names.map((name, i) => ["fn" + (i + 1), name])) };
  return template.replace(/\{\{(\w+)\}\}/g, (_, key) => {
    if (!Object.hasOwn(values, key)) throw Error("Unknown placeholder " + key + " in " + id);
    return values[key];
  });
}
for (const entry of referenceEntries) {
  if (!entry.id || documents.has(entry.id)) throw Error("Duplicate or empty document id: " + entry.id);
  if (strict.has(entry.category) && !entry.description) throw Error("Missing description: " + entry.id);
  const variants = entry.variants.map(v => ({
    key: v.key, label: typeNames[v.key] ?? v.key,
    symbols: v.names, signatures: v.names.map(name => signature(name, entry.id)),
    note: (entry.notes.get(v.key) ?? []).join("\n").trim(),
    example: expand((entry.examples.get(v.key) ?? []).join("\n").trimEnd() || entry.example, v, entry.id)
  }));
  const signatures = entry.members.map(name => signature(name, entry.id));
  const example = variants[0]?.example ?? entry.example;
  if (strict.has(entry.category) && !example) console.warn("Missing example: " + entry.id);
  documents.set(entry.id, { id: entry.id, category: entry.category, title: entry.title,
    description: entry.description, keywords: entry.keywords,
    signatures: [...signatures, ...variants.flatMap(v => v.signatures)], example,
    ...(variants.length ? { variants, commonSignatures: signatures } : {}) });
}
const pending = [];
for (const declaration of definitions.values()) {
  if (!declaration.symbols.length || covered.has(declaration.id)) continue;
  if (strict.has(declaration.category)) throw Error("Undocumented public symbol: " + declaration.id);
  pending.push(declaration.id);
  documents.set(declaration.id, { id: declaration.id, category: declaration.category,
    title: declaration.id, description: "", example: "", keywords: [], signatures: [declaration.signature] });
}
const used = new Set();
function navigation(nodes) {
  if (nodes.length > 5) console.warn("More than five categories: " + nodes.map(n=>n.title).join(", "));
  return nodes.map(node => {
    if (node.children) return { title: node.title, children: navigation(node.children) };
    const ids = node.entries ?? [...documents.values()].filter(d => d.category === node.legacyCategory)
      .sort((a,b) => Number(a.title===a.id)-Number(b.title===b.id)).map(d=>d.id);
    if (!ids.length) throw Error("Empty section: " + node.title);
    for (const id of ids) {
      if (!documents.has(id)) throw Error("Unknown document: " + id);
      used.add(id);
    }
    return { title: node.title, entries: ids };
  });
}
const sections = navigation(layout.sections);
for (const id of documents.keys()) if (!used.has(id)) throw Error("Document not in navigation: " + id);
if (pending.length) console.warn("Documentation migration pending for " + pending.length + " public symbols outside migrated categories.");
// Preserve v1 categories for readers shipped with older Companion versions.
const reference = categories.map(title => ({title, entries: [...documents.values()]
  .filter(d => d.category === title).map(({category, ...d})=>d)})).filter(c=>c.entries.length);
const notes = layout.notes.join("\n\n");
await fs.rm(output, { recursive: true, force: true });
await fs.mkdir(output, { recursive: true });
await fs.writeFile(path.join(output, "manifest.json"), JSON.stringify({
  schemaVersion: 2, ...config,
  components: [...ordered.values()].map(({ id, symbols, requires, code }) => ({ id, symbols, requires, code }))
}, null, 2) + "\n");
await fs.writeFile(path.join(output, "reference.json"), JSON.stringify({ schemaVersion: 1, version, notes, categories: reference, sections }, null, 2) + "\n");
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/CC-BY-4.0.txt"), path.join(output, "CC-BY-4.0.txt"));
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/MIT-0.txt"), path.join(packageDir, "LICENSE"));
console.log("Packed " + ordered.size + " C declarations into manifest v2.");
