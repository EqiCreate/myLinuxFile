import React, {  FormEvent,  useState } from 'react';
import styles from './Upload.module.css';
import { ToastContainer, toast } from 'react-toastify';

export default function FileUploader() {
  const [selectedFile, setSelectedFile] = useState<File|null>(null);

  const handleFileSelect = (event:React.ChangeEvent<HTMLInputElement>) => {
    // setSelectedFile(event.target.files[0]);
    if(event.target.files!=null){
        let path=event.target.files.item(0);
        setSelectedFile(path);
    }

  };

  const handleUpload = (event:FormEvent) => {
    event.preventDefault();

    const formData = new FormData();
    if(selectedFile!=null)
    {
        formData.append('file', selectedFile);

        fetch('http://192.168.3.61:7268/FileUpload', {
          method: 'POST',
          body: formData,
        })
          .then((response) => {
            toast("File uploaded successfully");
            console.log('File uploaded successfully');
          })
          .catch((error) => {
            toast("Error uploading file");
            console.error('Error uploading file', error);
          });
    }
   
  };
  const [progress, setProgress] = useState<string>("0");

  const handleFileUpload = async () => {
    const formData = new FormData();
    if(selectedFile!=null){
      formData.append("file", selectedFile);
      const xhr = new XMLHttpRequest();
      xhr.open("POST", "http://192.168.3.61:7268/FileUpload/UploadwithRedis");
      xhr.upload.addEventListener("progress", (event) => {
        if (event.lengthComputable) {
          const percentage = (event.loaded / event.total) * 100;
          setProgress(percentage.toFixed(0));
        }
      });
      xhr.upload.addEventListener("load", () => {
        toast("File uploaded successfully");
        console.log("File uploaded successfully");
      });
      xhr.upload.addEventListener("error", () => {
        toast("Error uploading file");
        console.log("Error uploading file");
      });
      xhr.send(formData);
    }
  };
  

  return (
    <div className={styles.ViewForm} >
      {/* <form onSubmit={handleFileUpload}> */}
        <input type="file" onChange={handleFileSelect} />
        <button onClick={handleFileUpload}>Upload</button>
        {/* <button type="submit">Upload</button> */}
      {/* </form> */}
      <div>{progress}%</div>
    </div>
  );
}
