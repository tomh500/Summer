import os

def convert_crlf_to_lf_in_current_directory():
    current_dir = os.getcwd()
    for root, _, files in os.walk(current_dir):
        for file in files:
            if file.endswith('.cfg'):
                file_path = os.path.join(root, file)
                with open(file_path, 'rb') as f:
                    content = f.read()
                new_content = content.replace(b'\r\n', b'\n')
                if new_content != content:
                    with open(file_path, 'wb') as f:
                        f.write(new_content)
                    print(f"Converted: {file_path}")
                else:
                    print(f"Already LF: {file_path}")

if __name__ == "__main__":
    convert_crlf_to_lf_in_current_directory()
