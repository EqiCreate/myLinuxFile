import axios from "axios";
import { useState, useEffect } from "react";
import { VideoMeta } from "../../views/Studio/Studio";
import NotFoundPage from "../NotFoundPage/NotFoundPage";


const StoredFileList = () => {
  const [files, setFiles] = useState<VideoMeta[]|null>(null);

  useEffect(() => {
    // axios.get("http://192.168.3.61:7268/FileDownload/files")
    //   .then(response => {
    //     // let content =response.data as VideoMeta[];
    //     // let formatted_response=content.map((element)=>({
    //     //   ...element,
    //     //   cca2:element.cca2.replace("media/","")
    //     // }));
      
    //     setFiles(response.data);
    //   })
    //   .catch(error => {
    //     console.error(error);
    //   });
    update_after_fetch();

  }, []);

  const update_after_fetch=async()=>{
    let flag=false;
    await axios.get("http://192.168.3.61:7268/FileDownload/files")
      .then(response => {
        // let content =response.data as VideoMeta[];
        // let formatted_response=content.map((element)=>({
        //   ...element,
        //   cca2:element.cca2.replace("media/","")
        // }));
        flag=true;
        setFiles(response.data);
      })
      .catch(error => {
        console.error(error);
      });
      if(!flag){
        return <NotFoundPage />;
      }
  }

  return (
    <div>
      <h1>Files:</h1>
      <ul>

        {files !==null && files.map(file => (
          <li key={file.name.common}>
            <a href={`http://192.168.3.61:7268/FileDownload/files/${file.name.common}`} download={file.cca2}>{file.name.common}</a>
          </li>
        ))}
      </ul>
    </div>
  );
};

export default StoredFileList;
