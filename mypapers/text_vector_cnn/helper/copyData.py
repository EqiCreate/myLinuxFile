import os
import shutil

def copy_files(src_dir, dest_dir, max_files):
    files = os.listdir(src_dir)
    for i, file in enumerate(files):
        if i >= max_files:
            break
        src_file = os.path.join(src_dir, file)
        dest_file = os.path.join(dest_dir, file)
        if os.path.isfile(src_file):
            shutil.copy2(src_file, dest_file)

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    data_dir = os.path.join(parent_dir, 'data/thucnews')

    if not os.path.exists(data_dir):
        os.mkdir(data_dir)

    hs_dir = os.path.join(script_dir, 'THUCNews')
    if os.path.exists(hs_dir) and os.path.isdir(hs_dir):
        for sub_item in os.listdir(hs_dir):
            sub_item_path = os.path.join(hs_dir, sub_item)
            if os.path.isdir(sub_item_path):
                new_dir_path = os.path.join(data_dir, sub_item)
                if not os.path.exists(new_dir_path):
                    os.mkdir(new_dir_path)
                copy_files(sub_item_path, new_dir_path, 6500)

if __name__ == "__main__":
    main()
