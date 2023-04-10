
import { useState, useMemo } from "react";
import { Video } from "../../shared/types/BLL";
import { SelectedVideo } from "../SelectedVideo/SelectedVideo";
import { VideoTracks } from "../VideoTracks/VideoTracks";
import * as StudioAction from '../../store/actions/ViedeoAction'
import styles from './Page.module.css';

export const Page = ({ videos }: { videos: Video[] }) => {
  const [selectedVideo, setSelectedVideo] = useState<Video>(videos[0]);
  // const [savedVideo, setSavedVideo] = useState<Video>(videos[0]);

  const TrackSave =()=>{
    StudioAction.add({path:selectedVideo.Url});
  }
  const list = useMemo(() => {
   return(
     <VideoTracks videos={videos} 
        onChanged={(v)=>{setSelectedVideo(v)}}
        /> 
   );
  }, [videos]);

  const selected = useMemo(() => {
    return (
      <SelectedVideo
        video={selectedVideo}
        onSaved={TrackSave}
      />
    );
  }, [selectedVideo]);


  return (
    <div>
      <h1>Videos</h1>
      <div className={styles.contentCss}>
        <div className={styles.ListConent}>
        {list}
        </div>
        <div className={styles.SelectedConent}>
          {selected}
        </div>
      </div>
      </div>
  );
};


