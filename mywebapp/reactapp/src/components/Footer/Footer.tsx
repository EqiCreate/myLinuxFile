

import React, { useCallback } from 'react';
import { useSelector } from 'react-redux';
import styles from './Footer.module.css';


const Footer: React.FC = () => {
    return (
      <footer className={styles.footer}>
            This is videos Tracks
      </footer>
    );
  };
  
  export default Footer;