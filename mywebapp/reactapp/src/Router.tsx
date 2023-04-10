import { RootState } from './store/reducers';
import { BrowserRouter, Route,unstable_HistoryRouter as HistoryRouter, Routes, Link } from 'react-router-dom';
import history from './lib/history'
import Studio from './views/Studio/Studio';
import Home from './views/Home/Home';
import { Provider } from 'react-redux';
import store from './store/store';
import App from './App';
import { Outlet } from 'react-router';
import Settings from './views/Settings/Settings';
import FileUploader from './components/Upload/Upload';

const RouterFC: React.FC = () => {
    // const conf = config.get() as Config;
    // const library = useSelector((state: RootState) => state.library);
    // const routes = useRoutes([
    //   {
    //     path: '/',
    //     element: <Home />,
    //   },
    //   {
    //     path: '/studio',
    //     element: <Studio />,
    //   },
    // ]);
      return (
            // <HistoryRouter history={history} >
            <BrowserRouter>
              <App>
                <Routes>
                  <Route path='/home' element={<Home/>}></Route>
                  <Route path='/studio' element={<Studio/>}></Route>
                  <Route path='/settings' element={<Settings />}>
                    {/* <Route path='library' element={<div><p>this is sss</p></div>} /> */}
                    <Route path='about' element={<div><p>this is about</p></div>} />
                    <Route path='upload' element={<FileUploader />} />

                </Route>
                </Routes>
              </App>
            {/* </HistoryRouter> */}
           </BrowserRouter>
      );
  };
  export default RouterFC;