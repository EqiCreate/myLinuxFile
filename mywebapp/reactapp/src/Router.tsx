import App from './App';
import { RootState } from './store/reducers';
import { BrowserRouter,Routes, Route, unstable_HistoryRouter as HistoryRouter } from 'react-router-dom';
import history from './lib/history'
import Studio from './views/Studio/Studio';
import Home from './views/Home/Home';

const Router: React.FC = () => {
    // const conf = config.get() as Config;
    // const library = useSelector((state: RootState) => state.library);
  
      return (
        //   <HistoryRouter  history={history}>
        <BrowserRouter>
               <App>
                  <Routes>
                    <Route path='/home' element={<Home/>}></Route>
                      <Route path='/studio' element={<Studio/>}></Route>
                      {/* <Route path='/settings' element={<SettingsView/>}>
                          <Route path='library' element={<SettingsLibrary library={library}/>} />
                          <Route path='interface' element={<SettingsUI config={conf}/>} />
                          <Route path='audio' element={<SettingsAbout />} />
                          <Route path='about' element={<SettingsAbout />} />
                      </Route>
                      <Route path='/band' element={<BandView/>}></Route> */}
                  </Routes>
               </App>
        {/* //   </HistoryRouter> */}
        </BrowserRouter>
      );
  };
  export default Router;