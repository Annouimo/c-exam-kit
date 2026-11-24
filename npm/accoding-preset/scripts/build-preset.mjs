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
    const signature = body.startsWith("#") || typeDefinition(body)
      ? code.slice(start, end).split("\n")[0].trim()
      : code.slice(start, start + body.search(/[;{]/)).trim().replace(/^static\s+(?:inline\s+)?/, "") + ";";
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
      let title, members;
      const details = [], example = [];
      for (const line of lines) {
        const [, tag, value = ""] = /^@(\w+)(?: (.*))?$/.exec(line) ?? [];
        switch (tag) {
          case "public": publiclyAvailable = true; break;
          case "requires": required = value.split(/\s+/).filter(Boolean); break;
          case "category": category = value; break;
          case "title": title = value; break;
          case "members": members = value.split(/\s+/).filter(Boolean); break;
          case "example": example.push(value); break;
          case undefined: details.push(line); break;
          default: throw Error("Unknown annotation @" + tag + " in " + file);
        }
      }
      if (title) referenceEntries.push({ id: entry.name, category, title,
        members: members ?? [entry.name], description: details.join("\n").trim(),
        example: example.join("\n") });
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

const covered = new Set();
const categories = ["输入和输出", "数组", "字符串", "数学、数位与二分", "大整数、枚举及其他类型"];
for (const entry of referenceEntries) {
  for (const name of entry.members) {
    if (!definitions.get(name)?.symbols.length) throw Error("Unknown public function in " + entry.id + ": " + name);
    covered.add(name);
  }
}
for (const declaration of definitions.values()) {
  if (!declaration.symbols.length || covered.has(declaration.id)) continue;
  referenceEntries.push({ id: declaration.id, category: declaration.category, title: declaration.id,
    members: [declaration.id], description: "", example: "" });
}
const reference = categories.map(title => ({ title, entries: referenceEntries
  .filter(entry => entry.category === title)
  .sort((a, b) => Number(a.title === a.id) - Number(b.title === b.id))
  .map(({id, title, members, description, example}) => ({id, title, description, example,
    signatures: members.map(name => definitions.get(name).signature)}))
})).filter(category => category.entries.length);
const notes = "读取同一份输入时统一使用这些读入接口，输出可照常用 printf。下标从 0 开始，区间 [l, r) 包含 l、排除 r。保留 main.c 中已有的头文件引用。"
  + "数组作为参数传入自定义函数后会变成指针，此时可用对应的 *_into 接口显式传入容量。";
await fs.rm(output, { recursive: true, force: true });
await fs.mkdir(output, { recursive: true });
await fs.writeFile(path.join(output, "manifest.json"), JSON.stringify({
  schemaVersion: 2, ...config,
  components: [...ordered.values()].map(({ id, symbols, requires, code }) => ({ id, symbols, requires, code }))
}, null, 2) + "\n");
await fs.writeFile(path.join(output, "reference.json"), JSON.stringify({ schemaVersion: 1, version, notes, categories: reference }, null, 2) + "\n");
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/CC-BY-4.0.txt"), path.join(output, "CC-BY-4.0.txt"));
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/MIT-0.txt"), path.join(packageDir, "LICENSE"));
console.log("Packed " + ordered.size + " C declarations into manifest v2.");
