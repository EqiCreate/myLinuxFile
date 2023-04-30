import React, {  ChangeEventHandler, FormEvent,  useState } from 'react';
import styles from './Upload.module.css';
import { ToastContainer, toast } from 'react-toastify';
import axios from 'axios';
import { isNum } from 'react-toastify/dist/utils';

export default function FileUploader() {
  const [selectedFile, setSelectedFile] = useState<File|null>(null);
  const [files, setFiles] = useState<File[]>([]);
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

  const [uploadProgress, setUploadProgress] = useState(0);
  const handleFilesUpload=async()=>{
    setUploadProgress(0);
    const xhr = new XMLHttpRequest();
    const formData = new FormData();
    files.forEach((file) => {
      formData.append('files', file);
    });
    xhr.open('POST', 'http://192.168.3.61:7268/FileUpload/multi-file', true);
    xhr.upload.addEventListener('progress', (event) => {
      const progress = Math.round((event.loaded / event.total) * 100);
      setUploadProgress(progress);
    });
    xhr.onreadystatechange = () => {
      if (xhr.readyState === 4) {
        if(xhr.status === 200){
          const response = JSON.parse(xhr.responseText);
          console.log(response);
          toast("success uploading file");
        }
        else{
          toast(xhr.responseText);
        }
      }
     
    };

    xhr.upload.addEventListener("error", () => {
      toast("Error uploading file");
    });
    xhr.send(formData);

  }

  const handleFilesSelect =(event :React.ChangeEvent<HTMLInputElement>)=>{
    if(event.target.files!=null)
    {
      const selectedFiles = Array.from(event.target.files);
      setFiles(selectedFiles);  
    }
      
  }

  const handleFilesUploadbyAsio=async()=>{
    setUploadProgress(0);
    const formData = new FormData();
    files.forEach((file) => {
      formData.append('files', file);
    });
    try {
      const response= await axios.post('http://192.168.3.61:7268/FileUpload/multi-file', formData,{
        timeout:120000,
      headers:{
        'Content-Type': 'multipart/form-data'
      },
      onUploadProgress:(progressEvent)=>{
        const { loaded, total } = progressEvent;
        if(typeof total ==='number')
        {
          const percentCompleted = Math.round((loaded * 100) / total);
          setUploadProgress(percentCompleted);
        }
      }
    }).then(response=>{
      toast("success uploading file");
    }).catch(error=>{
      toast(error);
    });
    } catch (error) {
      toast("error");
      console.error(error);
    }
  }

  return (
    <div className={styles.ViewForm} >
        <input type="file" multiple onChange={handleFilesSelect} />
        <button onClick={handleFilesUploadbyAsio}>Upload</button>
      {uploadProgress > 0 && (
        <progress value={uploadProgress} max="100">
          {uploadProgress}%
        </progress>
      )}
    </div>
  );
}

