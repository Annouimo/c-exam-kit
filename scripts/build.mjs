import fs from "node:fs/promises";
import path from "node:path";

const root = path.resolve(import.meta.dirname, "..");
const packageDir = path.join(root, "dist", "package");
const output = path.join(packageDir, "preset");
const config = JSON.parse(await fs.readFile(path.join(root, "kit.json"), "utf8"));
const pkg = JSON.parse(await fs.readFile(path.join(root, "package.json"), "utf8"));
const {version} = pkg;
const vocabulary = new Set("输入 输出 数值 整数 浮点数 数组 字符串 结构体 大整数 复数 日期 时刻 位 构造 修改 排序 查找 统计 数学计算 转换 区间 二分 枚举 判断".split(" "));
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

const componentDirectory = path.join(root, "src");
const files = (await fs.readdir(componentDirectory)).filter(file => file.endsWith(".h")).sort();
const definitions = new Map();
const referenceEntries = [];
for (const file of files) {
  const input = await fs.readFile(path.join(componentDirectory, file), "utf8");
  for (const entry of declarations(input, file)) {
    if (definitions.has(entry.name)) throw Error("Duplicate declaration: " + entry.name);
    let required = [], publiclyAvailable = false;
    for (const block of entry.documentation) {
      const lines = block.split("\n");
      let title, summary, returns, complexity, members, id = entry.name, keywords = [], related = [], tags = [];
      const parameters=[], requirements=[], errors=[];
      const details = [], examples = new Map(), variants = [], notes = new Map();
      let section = details;
      for (const line of lines) {
        const [, tag, value = ""] = /^@(\w+)(?: (.*))?$/.exec(line) ?? [];
        switch (tag) {
          case "public": publiclyAvailable = true; break;
          case "requires": required = value.split(/\s+/).filter(Boolean); break;
          case "category": break; // Non-document legacy marker; remove when touching the component.
          case "id": id = value; break;
          case "title": title = value; break;
          case "tags": tags = value.trim().split(/\s+/).filter(Boolean); break;
          case "summary": summary = value; break;
          case "param": { const m = /^([A-Za-z_]\w*)\s+(.+)$/.exec(value); if(!m) throw Error("Invalid parameter in "+id); parameters.push({name:m[1],description:m[2]}); break; }
          case "return": returns = value; break;
          case "pre": requirements.push(value); break;
          case "error": errors.push(value); break;
          case "complexity": complexity = value; break;
          case "keywords": keywords = value.split(/\s+/).filter(Boolean); break;
          case "related": related = value.split(/\s+/).filter(Boolean); break;
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
      if (title) referenceEntries.push({ id, title, summary, tags, keywords, related, parameters, requirements, errors, returns, complexity,
        members: members ?? (variants.length ? [] : [entry.name]),
        description: details.join("\n").trim(),
        example: (examples.get("") ?? []).join("\n").trimEnd(),
        variants, examples, notes });
    }
    if (publiclyAvailable && entry.callback) throw Error("Callback declaration marked public: " + entry.name);
    definitions.set(entry.name, { id: entry.name, symbols: publiclyAvailable ? [entry.name] : [],
      requires: required, code: entry.code, signature: entry.signature });
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

const covered = new Set();
const documents = new Map();
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
  if (!entry.summary || !entry.description) throw Error("Missing summary or description: " + entry.id);
  if(!entry.tags.length || entry.tags.length>4 || new Set(entry.tags).size!==entry.tags.length || entry.tags.some(t=>!vocabulary.has(t))) throw Error("Invalid tags in "+entry.id);
  const variants = entry.variants.map(v => ({
    key: v.key, label: typeNames[v.key] ?? v.key,
    symbols: v.names, signatures: v.names.map(name => signature(name, entry.id)),
    advancedSignatures: v.names.filter(name=>name.endsWith("_into")).map(name=>definitions.get(name).signature),
    note: (entry.notes.get(v.key) ?? []).join("\n").trim(),
    example: expand((entry.examples.get(v.key) ?? []).join("\n").trimEnd() || entry.example, v, entry.id)
  }));
  const signatures = entry.members.map(name => signature(name, entry.id));
  const advancedSignatures = entry.members.filter(name=>name.endsWith("_into")).map(name=>definitions.get(name).signature);
  const example = variants[0]?.example ?? entry.example;
  if (!example || variants.some(v=>!v.example)) throw Error("Missing example: " + entry.id);
  const allSignatures=[...signatures,...variants.flatMap(v=>v.signatures)];
  if(new Set(entry.parameters.map(p=>p.name)).size!==entry.parameters.length) throw Error("Duplicate parameters in "+entry.id);
  for(const p of entry.parameters) if(!allSignatures.some(s=>s.match(/[A-Za-z_]\w*/g)?.includes(p.name))) throw Error("Parameter not in signature: "+entry.id+" "+p.name);
  documents.set(entry.id, { id: entry.id, title: entry.title, summary: entry.summary, tags: entry.tags,
    description: entry.description, keywords: [...new Set(entry.keywords)], related: entry.related,
    ...(entry.parameters.length ? {parameters:entry.parameters}:{}), ...(entry.returns?{returns:entry.returns}:{}),
    ...(entry.requirements.length?{requirements:entry.requirements}:{}), ...(entry.errors.length?{errors:entry.errors}:{}), ...(entry.complexity?{complexity:entry.complexity}:{}),
    advancedSignatures,
    signatures: [...signatures, ...variants.flatMap(v => v.signatures)], example,
    ...(variants.length ? { variants, commonSignatures: signatures } : {}) });
}
for (const declaration of definitions.values()) {
  if (declaration.symbols.length && !covered.has(declaration.id))
    throw Error("Undocumented public symbol: " + declaration.id);
}
for (const doc of documents.values()) {
  for (const id of doc.related) {
    if (id === doc.id || !documents.has(id)) throw Error("Invalid related entry " + id + " in " + doc.id);
  }
}
const entries=[...documents.values()];
const tags=[...new Set(entries.flatMap(e=>e.tags))].sort();
const notes=(await fs.readFile(path.join(root,"GUIDE.md"),"utf8")).trim();
console.log("Reference: "+entries.length+" documented operations, "+covered.size+" public symbols, "+tags.length+" tags.");
await fs.rm(packageDir, { recursive: true, force: true });
await fs.mkdir(output, { recursive: true });
await fs.writeFile(path.join(output, "manifest.json"), JSON.stringify({
  schemaVersion: 2, ...config,
  components: [...ordered.values()].map(({ id, symbols, requires, code }) => ({ id, symbols, requires, code }))
}, null, 2) + "\n");
await fs.writeFile(path.join(output, "reference.json"), JSON.stringify({ schemaVersion: 2, version, notes, tags, entries }, null, 2) + "\n");
await fs.copyFile(path.join(root,"LICENSES/CC-BY-4.0.txt"), path.join(output, "CC-BY-4.0.txt"));
await fs.copyFile(path.join(root,"LICENSES/MIT-0.txt"), path.join(packageDir, "LICENSE"));
const staged={...pkg};delete staged.private;delete staged.scripts;delete staged.repository.directory;
await fs.writeFile(path.join(packageDir,"package.json"),JSON.stringify(staged,null,2)+"\n");
console.log("Packed " + ordered.size + " C declarations into manifest v2.");
