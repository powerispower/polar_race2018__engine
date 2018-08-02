package com.alibabacloud.polar_race.engine.common;

import org.rocksdb.Options;
import org.rocksdb.RocksDB;
import org.rocksdb.RocksDBException;
import org.rocksdb.RocksIterator;

import com.alibabacloud.polar_race.engine.common.AbstractEngine;
import com.alibabacloud.polar_race.engine.common.exceptions.EngineException;
import com.alibabacloud.polar_race.engine.common.exceptions.RetCodeEnum;

public class EngineRace extends AbstractEngine {

	@Override
	public void open(String name) throws EngineException {
	
  }
	
	@Override
	public void write(String key, String value) throws EngineException {
		
	}
	
	@Override
	public String read(String key) throws EngineException {
		String value = null;
		
		return value;
	}
	
	@Override
	public void range(String lower, String upper, AbstractVisitor visitor) throws EngineException {
		
	}
	
	@Override
	public void close() {
	
  }

}
