/**
 * Redux
 */
export interface Action {
    // TODO action specific types
    type: string;
    payload?: any;
  }

  export enum PlayerStatus {
    PLAY = 'play',
    PAUSE = 'pause',
    STOP = 'stop',
  }


/**
 * Untyped libs / helpers
 */
 export type LinvoSchema<Schema> = {
    _id: string;
    find: any;
    findOne: any;
    insert: any;
    copy: any; // TODO better types?
    remove: any;
    save: any;
    serialize: any;
    update: any;
    ensureIndex: any;
    // bluebird-injected
    findAsync: any;
    findOneAsync: any;
    insertAsync: any;
    copyAsync: any;
    removeAsync: any;
    saveAsync: any;
    serializeAsync: any;
    updateAsync: any;
  } & {
    [Property in keyof Schema]: Schema[Property];
  };

  export interface Track {
    // album: string;
    // artist: string[];
    // disk: {
    //   no: number;
    //   of: number;
    // };
    // duration: number;
    // genre: string[];
    // loweredMetas: {
    //   artist: string[];
    //   album: string;
    //   title: string;
    //   genre: string[];
    // };
    path: string;
    // playCount: number;
    title?: string;
    // track: {
    //   no: number;
    //   of: number;
    // };
    // year: number | null;
  }
 export type TrackModel = LinvoSchema<Track>;

 export type Video = {
  __typename: "video";
  name: string;
  // id: string;
  Url: string;
  // independent: boolean;
  // unMember: boolean;
  // region: string;
  // capital: string;
  // subregion: string;
};
