import { RootState } from './store/reducers';
import { BrowserRouter,Router, Route, unstable_HistoryRouter as HistoryRouter, Routes } from 'react-router-dom';
import history from './lib/history'
import Studio from './views/Studio/Studio';
import Home from './views/Home/Home';
import { Provider } from 'react-redux';
import store from './store/store';
import App from './App';

const RouterFC: React.FC = () => {
    // const conf = config.get() as Config;
    // const library = useSelector((state: RootState) => state.library);
  
      return (
            <HistoryRouter history={history} >
              <App>
                <Routes>
                  <Route path='/home' element={<Home/>}></Route>
                  <Route path='/studio' element={<Studio/>}></Route>
                </Routes>
              </App>
            </HistoryRouter>
      );
  };
  export default RouterFC;