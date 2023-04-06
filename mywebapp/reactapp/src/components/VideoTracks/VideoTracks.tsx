import { Video } from "../../shared/types/BLL";

type ItemProps = {
    video: Video;
  onItemClick: () => void;
};

const Item = ({ video, onItemClick }: ItemProps) => {

  console.log("render");

  return (
    <button onClick={onItemClick}>
      <img
        src={video.Url}
        width={50}
        style={{ marginRight: "8px" }}
        alt={video.name}
      />
      <div>{video.name}</div>
    </button>
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
    <div className="countries-list">
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
