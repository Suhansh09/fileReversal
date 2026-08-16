# File Verification

The verification component checks whether the generated output file was
processed correctly according to the selected reversal mode. It also
checks file sizes and reports relevant file and directory information.

## Full File Verification

Full-file verification checks whether the generated output represents the
complete reversal of the original input. The verification compares the
output against the expected reversed data while also checking that both
files have the same size.

## Block Reversal Verification

Block reversal verification checks each block independently. The expected
reversed contents of every block are compared with the corresponding
portion of the generated output to ensure that the block boundaries and
ordering are preserved.

## Partial Reversal Verification

Partial reversal verification checks the different regions of the file
separately. The selected range is checked according to the expected
partial-reversal behavior, while the remaining regions are checked to
ensure their expected ordering is preserved.
