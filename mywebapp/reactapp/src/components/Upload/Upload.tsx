import React, { ChangeEvent, ChangeEventHandler, EventHandler, FormEvent, FormEventHandler, useState } from 'react';


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
            console.log('File uploaded successfully');
          })
          .catch((error) => {
            console.error('Error uploading file', error);
          });
    }
   
  };

  return (
    <div>
      <form onSubmit={handleUpload}>
        <input type="file" onChange={handleFileSelect} />
        <button type="submit">Upload</button>
      </form>
    </div>
  );
}
