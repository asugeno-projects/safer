/**
* @file       analysisCommandPaser.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/07 Akihiro Sugeno
*       -# Initial Version
*/

#include "../../parser/command/analysisCommandParser.h"

/**
* @fn void AnalysisCommandParser::setCommandType(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �R�}���h�^�C�v�̃Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �R�}���h�^�C�v���ʎq
* @param attr �����l
* @details �R�}���h�^�C�v��analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::setCommandType(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setCommandType(CommandTypeLabelList[value]);
}

/**
* @fn void AnalysisCommandParser::setCommandName(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �R�}���h���̃Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �R�}���h��
* @param attr �����l
* @details �R�}���h����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::setCommandName(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setCommandName(value);
}

/**
* @fn void AnalysisCommandParser::addTarget(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �����Ώۂ̃Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �����Ώێ��ʎq
* @param attr �����l
* @details �����Ώۂ�analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::addTarget(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->addTarget(targetList[value]);
}

/**
* @fn void AnalysisCommandParser::addValue(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �����Ɏg�p���镶����̃Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �����Ɏg�p���镶����
* @param attr �����l
* @details �����Ɏg�p���镶�����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::addValue(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	if (value.length() != 0)
	{
		analysisCommand->addValue(value);
	}
}

/**
* @fn void AnalysisCommandParser::setOutputMessage(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief ���o���ɕ\�����郁�b�Z�[�W�̃Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �\�����b�Z�[�W
* @param attr �����l
* @details ���o���ɕ\�����郁�b�Z�[�W��analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::setOutputMessage(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setOutputMessage(value);
}

/**
* @fn void AnalysisCommandParser::setAlertLevel(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief ���o���̃A���[�g���̃Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value ���o���̃A���[�g��񎯕ʎq
* @param attr �����l
* @details ���o���̃A���[�g����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::setAlertLevel(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setMessageAlertLevel(AlertLevelLabelList[value]);
}

/**
* @fn void AnalysisCommandParser::setEnableFlg(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief ��͖��ߗL���t���O�Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value ��͖��ߗL���t���O
* @param attr �����l
* @details ��͖��ߗL����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::setEnableFlg(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setEnableFlg(boost::lexical_cast<bool>(value));
}

/**
* @fn void AnalysisCommandParser::addTargetExtractionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �����e�[�u�����o�p���K�\���p�^�[���Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �����e�[�u�����o�p���K�\���p�^�[��������
* @param attr �����l
* @details �����e�[�u�����o�p���K�\���p�^�[����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::addTargetExtractionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	if (value.length() != 0)
	{
		ExtractionParam extractionParam;
		extractionParam.targetName = value;
		extractionParam.section = attr[SECTION_ATTRIBUTE];
		analysisCommand->addTargetExtractionParam(extractionParam);
	}
}

/**
* @fn void AnalysisCommandParser::addTableOptionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �e�[�u���I�v�V�������萳�K�\���p�^�[���Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �e�[�u���I�v�V�������蕶����
* @param attr �����l
* @details �e�[�u���I�v�V��������p���K�\���p�^�[����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::addTableOptionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	TableOption tableOption;
	tableOption.tableOption = TableOptionTypeItemList[value];
	tableOption.logicalOperation = StringToLogicalOperationType[attr[LOGICAL_OPERATION_ATTRIBUTE]];
	((AnalysisCommandTypeCheck *)analysisCommand)->addTableOption(tableOption);
}

/**
* @fn void AnalysisCommandParser::addDataTypeParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief �f�[�^�^����p���K�\���p�^�[���Z�b�g�֐�
* @param analysisCommand ��͖��߃R�}���h�C���X�^���X
* @param value �f�[�^�^����p������
* @param attr �����l
* @details �f�[�^�^����p���K�\���p�^�[����analysisCommand�ɓo�^���܂��B
*/
void AnalysisCommandParser::addDataTypeParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	DataType dataType;
	dataType.dataType = value;
	dataType.logicalOperation = StringToLogicalOperationType[attr[LOGICAL_OPERATION_ATTRIBUTE]];
	((AnalysisCommandTypeCheck *)analysisCommand)->addDateType(dataType);
}

/**
* @fn void AnalysisCommandParser::setParams()
* @brief ��͖��߃C���X�^���X�p�����[�^�Z�b�g�֐���map��񐶐��֐�
* @details ��͖��߃C���X�^���X�̃p�����[�^�Z�b�g�֐���map�ϐ��ɓo�^���܂��B
*/
void AnalysisCommandParser::setParams()
{
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TYPE, AnalysisCommandParser::setCommandType));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_NAME, AnalysisCommandParser::setCommandName));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TARGET, AnalysisCommandParser::addTarget));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_VALUE, AnalysisCommandParser::addValue));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_OUTPUT_MESSAGE, AnalysisCommandParser::setOutputMessage));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_ALERT_LEVEL, AnalysisCommandParser::setAlertLevel));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_ENABLE_FLG, AnalysisCommandParser::setEnableFlg));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TARGET_EXTRACTION_PARAM, AnalysisCommandParser::addTargetExtractionParam));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TABLE_OPTION, AnalysisCommandParser::addTableOptionParam));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_DATA_TYPE, AnalysisCommandParser::addDataTypeParam));
}