import fs from "node:fs/promises";
import path from "node:path";
const packageDir = path.resolve(import.meta.dirname, "..");
const source = path.resolve(packageDir, "../../scaffold/presets/exam-io-c99");
const output = path.join(packageDir, "preset");
const manifest = JSON.parse(await fs.readFile(path.join(source,"manifest.json"),"utf8"));
if (manifest.schemaVersion !== 1 || manifest.language !== "c" ||
    manifest.standard !== "c99" || !Array.isArray(manifest.components))
  throw Error("Unsupported exam-io-c99 preset manifest");
for (const component of manifest.components) {
  if (!/^[A-Za-z0-9_/-]+\.h$/.test(component.source) ||
      component.source.startsWith("/") || component.source.includes(".."))
    throw Error("Unsafe component path: " + component.source);
  await fs.access(path.join(source,component.source));
}
await fs.rm(output,{recursive:true,force:true});
await fs.cp(source,output,{recursive:true});
await fs.copyFile(path.resolve(packageDir, "../../LICENSES/MIT-0.txt"),
  path.join(packageDir, "LICENSE"));
console.log("Packed exam-io-c99 preset with",manifest.components.length,"components.");
