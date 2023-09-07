import React, { useCallback, useEffect } from 'react';
import logo from './logo.svg';
import './App.css';
import 'video-react/dist/video-react.css'; // import css
import 'react-toastify/dist/ReactToastify.css';
import 'bootstrap/dist/css/bootstrap.min.css';
import { Outlet, useNavigate } from 'react-router';
import styles from './App.module.css';
import Header from './components/Header/Header';
import Footer from './components/Footer/Footer';

type Props = {
  children?: React.ReactNode
};

const appMain: React.FC <Props> = ({children}) => {
  // const navigate = useNavigate();
  // useEffect(() => {
  //     // AppActions.init();
  //   }, [navigate]);
  return (
      <div className={`${styles.root}`} >
          {/* 
          
          <footer>wtf</footer> */}
           <div className={`${styles.flexitem1}`}>
                <header className={styles.headerTest}>
                  <Header></Header></header>
           </div>
           <div className={`${styles.flexitem2}`}>
            <main className={styles.mainContent}>{children}</main>
           </div>
           <div className={`${styles.flexitem3}`}>
            <Footer></Footer>
           </div>
      </div>
  );
};
export default appMain;

// export default App;
