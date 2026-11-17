/* SPDX-License-Identifier: MIT-0 */
/* Integer [start, stop), like range. No signed overflow on the last increment. */
static @NAME@ @NAME@_range(@TYPE@ start, @TYPE@ stop, @TYPE@ step) {
    if (!step) tb_fail("range step must not be zero");
    @NAME@ result = @NAME@_new();
    @TYPE@ value = start;
    while (step > 0 ? value < stop : value > stop) {
        @NAME@_push(&result, value);
        if ((step > 0 && value > @MAX@ - step) ||
            (step < 0 && value < @MIN@ - step)) break;
        value += step;
    }
    return result;
}
