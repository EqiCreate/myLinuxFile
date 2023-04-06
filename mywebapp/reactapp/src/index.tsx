import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import reportWebVitals from './reportWebVitals';
import { Provider } from 'react-redux';
import Root from './Root';
// import Router from './Router';
import store from './store/store';
import RouterFC from './Router';

const root = ReactDOM.createRoot(
  document.getElementById('root') as HTMLElement
);
root.render(
  // <React.StrictMode>
    <Root>
      <Provider store={store}>

          <RouterFC></RouterFC>
        {/* <Router></Router> */}
      </Provider>
    {/* <Router history={}></Router> */}
      </Root>
  // </React.StrictMode>
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
