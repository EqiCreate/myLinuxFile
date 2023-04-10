import React from "react";
import { Video } from "../../shared/types/BLL";
import styles from './SelectedVideo.module.css';

export const SelectedVideo = ({
  video,
  onSaved
}: {
    video: Video;
    onSaved: () => void;
}) => {
  return (
    <div className={styles.selectedContainer}>
      <div className={styles.selectedInfo}>
        <ul>
          <li>Name: {video.name}</li>
          <li>Url: {video.Url}</li>
        </ul>
        <button
          className={styles.selectedButton}
          onClick={onSaved}
          type="button"
        >
          Play
        </button>
      </div>
     
      <p>{video.Url} </p>
    </div>
  );
};
