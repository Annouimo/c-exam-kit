import test from "node:test";
import assert from "node:assert/strict";
import {releaseDecision} from "./npm-release-state.mjs";

test("only new stable versions publish, never downgrade or overwrite", () => {
  assert.equal(releaseDecision("0.1.0", ["0.1.0-beta.2"]), true);
  assert.equal(releaseDecision("0.1.0", ["0.1.0"]), false);
  assert.equal(releaseDecision("0.1.10", ["0.1.9"]), true);
  assert.throws(() => releaseDecision("0.1.9", ["0.1.10"]));
  assert.throws(() => releaseDecision("0.1.0-beta.1", ["0.0.1"]));
  assert.throws(() => releaseDecision("0.1.0", []));
  assert.throws(() => releaseDecision("0.1.0", {error:"registry unavailable"}));
  assert.equal(releaseDecision("0.2.0-beta.1", ["0.1.0"], "next"), true);
  assert.equal(releaseDecision("0.2.0-beta.1", ["0.2.0-beta.1"], "next"), false);
});
