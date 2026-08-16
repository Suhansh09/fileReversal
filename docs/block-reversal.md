# Block Reversal

Block reversal divides the input file into fixed-size blocks and reverses
the contents within each block. The operation allows large files to be
processed in manageable sections.

## Block Size Handling

The block size determines how many bytes are processed together during
each reversal operation. A valid positive block size is required for
block-based processing.

## Final Block Handling

If the file size is not an exact multiple of the selected block size,
the remaining bytes form the final block. This final block is processed
separately without discarding any input data.

## Verification

Block reversal can be verified by checking each block independently.
The bytes within every processed block should appear in reverse order,
while all file data should remain accounted for in the resulting output.
