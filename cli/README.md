# Multi-threaded File System CLI

A command-line interface for interacting with the Multi-threaded File System.

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

Run the CLI program:

```bash
./mtfs_cli
```

## Available Commands

1. Create a file:

   ```
   > create-file example.txt
   ```

2. Write content to a file:

   ```
   > write-file example.txt Hello, this is a test file!
   ```

3. Read file content:

   ```
   > read-file example.txt
   ```

4. Delete a file:

   ```
   > delete-file example.txt
   ```

5. Create a directory:

   ```
   > create-dir my_folder
   ```

6. List directory contents:

   ```
   > list-dir my_folder
   ```

7. Get help:

   ```
   > help
   ```

8. Exit the program:
   ```
   > exit
   ```

## Example Session

Here's a typical session showing basic file operations:

```
> create-dir documents
Directory created successfully: documents

> create-file documents/note.txt
File created successfully: documents/note.txt

> write-file documents/note.txt This is a test note.
Content written successfully to: documents/note.txt

> read-file documents/note.txt
Content of documents/note.txt:
This is a test note.

> list-dir documents
Contents of directory documents:
  note.txt

> delete-file documents/note.txt
File deleted successfully: documents/note.txt

> exit
```

## Error Handling

The CLI provides error messages for common issues:

- Invalid commands
- Missing arguments
- File/directory not found
- Permission errors
- Other filesystem-related errors

All operations are logged using the system's logging facility.
