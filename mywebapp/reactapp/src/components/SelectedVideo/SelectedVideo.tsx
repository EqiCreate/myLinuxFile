import React from "react";
import { Video } from "../../shared/types/BLL";

export const SelectedVideo = ({
  video,
  onSaved
}: {
    video: Video;
    onSaved: () => void;
}) => {
  return (
    <div className="selected-container">
      <div className="selected-info">
        <ul>
          <li>Country: {video.name}</li>
          <li>Url: {video.Url}</li>
        </ul>
        <button
          className="selected-button"
          onClick={onSaved}
          type="button"
        >
          Save
        </button>
      </div>
      <p>{video.Url} </p>
    </div>
  );
};
