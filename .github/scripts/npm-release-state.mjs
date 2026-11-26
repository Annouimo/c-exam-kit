import { execFileSync } from "node:child_process";
import { appendFileSync } from "node:fs";
import { pathToFileURL } from "node:url";

const stable = /^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)$/;
const preview = /^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)-(alpha|beta|rc|dev)\.(0|[1-9]\d*)$/;

export function compareStable(a, b) {
  const x = a.split(".").map(BigInt), y = b.split(".").map(BigInt);
  for (let i=0; i<3; i++) if (x[i] !== y[i]) return x[i] > y[i] ? 1 : -1;
  return 0;
}

export function releaseDecision(version, versions, channel = "latest") {
  if (!(channel === "latest" ? stable : preview).test(version)) throw Error("Invalid release version/channel: " + version);
  if (!Array.isArray(versions) || !versions.length || !versions.every(v => typeof v === "string")) throw Error("Invalid registry versions response");
  if (versions.includes(version)) return false;
  if (channel === "latest" && versions.some(v => stable.test(v) && compareStable(v, version) > 0))
    throw Error("Refusing to publish a version older than an existing stable release: " + version);
  return true;
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const [pkg, version, channel = "latest"] = process.argv.slice(2);
  if (!/^@routilet\/[a-z0-9-]+$/.test(pkg)) throw Error("Unexpected package name");
  const raw = execFileSync("npm", ["view", pkg, "versions", "--json", "--registry=https://registry.npmjs.org", "--fetch-retries=2", "--fetch-timeout=15000"], { encoding:"utf8", timeout:90000 });
  const versions = JSON.parse(raw);
  const publish = releaseDecision(version, typeof versions === "string" ? [versions] : versions, channel);
  console.log(pkg + "@" + version + (publish ? " is not published" : " already published; skipping"));
  appendFileSync(process.env.GITHUB_OUTPUT, "publish=" + publish + "\n");
}
