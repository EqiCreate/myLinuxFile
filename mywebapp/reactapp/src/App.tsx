import React, { useCallback, useEffect } from 'react';
import logo from './logo.svg';
import './App.css';
import 'video-react/dist/video-react.css'; // import css
import { useNavigate } from 'react-router';
import styles from './App.module.css';
import Header from './components/Header/Header';


// function App() {
//   return (
//     <div className="App">
//       {/* <header className="App-header"> */}
//         {/* <img src={logo} className="App-logo" alt="logo" />
//         <p>
//           Edit <code>src/App.tsx</code> and save to reload.
//         </p>
//         <a
//           className="App-link"
//           href="https://reactjs.org"
//           target="_blank"
//           rel="noopener noreferrer"
//         >
//           Learn React
//         </a> */}
//       {/* </header> */}
//       <Player>
//       <source src="https://media.w3.org/2010/05/sintel/trailer_hd.mp4" />
//     </Player>
//     </div>
//   );
// }

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
          <header className={styles.headerTest}>Header</header>
            <Header></Header>  
          <main className={styles.mainContent}>{children}</main>
          {/* <Footer></Footer> */}
      </div>
  );
};
export default appMain;

// export default App;
