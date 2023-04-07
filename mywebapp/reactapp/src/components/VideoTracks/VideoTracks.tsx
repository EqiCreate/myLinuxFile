import { Video } from "../../shared/types/BLL";
import styles from './VideoTracks.module.css';

type ItemProps = {
    video: Video;
  onItemClick: () => void;
};

const Item = ({ video, onItemClick }: ItemProps) => {

  console.log("render");

  return (
    <div className={styles.ViewItem}>
    <button onClick={onItemClick}>
      <img
        width={50}
        style={{ marginRight: "8px" }}
        alt={video.name}
      />
      <div>{video.name}</div>
    </button>
    </div>
  );
};

type VideoListProps = {
    videos: Video[];
  onChanged: (country: Video) => void;
};

export const VideoTracks = ({
  videos,
  onChanged,
  // savedVideo
}: VideoListProps) => {
  return (
    <div className={styles.ViewTrack}>
      {videos.map((video) => (
        <Item 
        key={video.name}
          video={video}
          onItemClick={() => onChanged(video)}
        />
      ))}
    </div>
  );
};
