/**
* @file       columnDataTypeChecker.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/15 Akihiro Sugeno
*       -# Initial Version
*/

#include "columnDataTypeChecker.h"
#include "../static_analytics/compRegularExpression.h"
#include "../analysis_command/analysisCommand.h"
#include <list>
#include <map>
#include <regex>

/**
* @fn void ColumnDataTypeChecker::run(AnalysisCommandTypeCheck *analysisCommand, ER *er)
* @brief ��͏������s�֐�(Column�̃f�[�^�^����)
* @param analysisCommand ��͖��߃C���X�^���X
* @param er ER���i�[�C���X�^���X�̃|�C���^�ϐ�
* @details ��͏����̎��s���s���B<br>�f�[�^�^�����o�Ώۂ̂��̂��𔻒肵�A���o�Ώۂł����repoter�ɕ񍐏���ǉ�����
*/
void ColumnDataTypeChecker::run(AnalysisCommandTypeCheck *analysisCommand, ER *er)
{
	//����p�f�[�^�^���X�g���擾
	std::list<DataType> dataTypes = analysisCommand->getDataTypes();
	//����p�e�[�u���I�v�V�������X�g���擾
	std::list<TableOption> tableOptions = analysisCommand->getTableOptions();
	
	//�Ώۃe�[�u������p���K�\���p�^�[���C���X�^���X�̐���
	std::wregex extractPatternForTable = CompRegularExpression::getExtractPattern(analysisCommand->getTargetExtractionParams(SECTION_TABLE));
	std::wregex extractPatternForTab = CompRegularExpression::getExtractPattern(analysisCommand->getTargetExtractionParams(SECTION_TAG));
	//Column���o���K�\���p�^�[���C���X�^���X�̐���
	std::wregex columnPattern = CompRegularExpression::getStringsToWOrPattern(analysisCommand->getValues());

	//Entity�̐��������[�v����
	for (std::list<Entity *>::iterator entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		//���o�Ώۂ̃e�[�u��������
		if (!regex_match((*entityIt)->getPhysicalTableName().c_str(), extractPatternForTable) &&
			!regex_match((*entityIt)->getTagName().c_str(), extractPatternForTab))
		{
			//�Ώۃe�[�u���łȂ��ꍇ�A�������X�L�b�v
			continue;
		}

		//Entity����Column�����擾
		auto fields = (*entityIt)->getfields();

		//Column�̐��������[�v����
		for (auto fieldIt = fields.begin(); fieldIt != fields.end(); fieldIt++)
		{
			//���o�ΏۃJ��������
			if (!regex_match((*fieldIt).name.physicalName.c_str(), columnPattern))
			{
				//�Ώ�Column����Ȃ��ꍇ�͔�΂�
				continue;
			}

			//�f�[�^�J�����̔��菈��
			if (!this->chackForDataTypeList(dataTypes, (*fieldIt).type))
			{
				//���o�ΏۂłȂ���Ώ����I��
				continue;
			}

			//Table Option����
			if (!this->chackFoTableOptionList(tableOptions, (*fieldIt).DDLOptions))
			{
				//���o�ΏۂłȂ���Ώ����I��
				continue;
			}

			//�����ɓ��B�������_�ŁA���o�ΏۂƂ݂Ȃ��A���|�[�g���֒~�ς���
			Reporter::getInstance()->addRecord(analysisCommand, CommandTypeList::E_TypeCheck, (*entityIt)->getPhysicalTableName(), (*fieldIt).name.physicalName);
		}
	}
}

/**
* @fn bool ColumnDataTypeChecker::checkForJudgment(int *commandCount, int *hitCount)
* @brief �R�}���h���s�񐔂ƃq�b�g�񐔂��r���A���o�Ώۂ����肷��֐�
* @param commandCount ���ߎ��s�񐔂̃J�E���^�[�p�z��
* @param hitCount ����񐔂̃J�E���^�[�p�z��
* @return ���ߎ��s�񐔃J�E���^�[�Ɣ���񐔃J�E���^�[���r���A<br>�������ꂽ�f�[�^�����o�Ώۂ��������̂����肵���ʂ�bool�^�ŕԂ�
* @details 
*/
bool ColumnDataTypeChecker::checkForJudgment(int *commandCount, int *hitCount)
{
	//����m�F
	if (commandCount[LogicalOperationTypeList::E_And] != hitCount[LogicalOperationTypeList::E_And])
	{
		//AND�������莎�s�񐔂Ɣ��萔����v���Ȃ��ꍇ�A�������B(false)
		return false;
	}
	if (commandCount[LogicalOperationTypeList::E_NotAnd] != hitCount[LogicalOperationTypeList::E_NotAnd])
	{
		//NOT AND�������莎�s�񐔂Ɣ��萔����v���Ȃ��ꍇ�A�������B(false)
		return false;
	}
	if (commandCount[LogicalOperationTypeList::E_Or] > 0 &&
		hitCount[LogicalOperationTypeList::E_Or] == 0)
	{
		//OR��������ɂāA���莎�s�񐔂�1�ȏォ����񐔂�1�ȏ�ł͖����Ƃ��A
		//�������B(false)
		return false;
	}
	if (commandCount[LogicalOperationTypeList::E_NotOr] > 0 &&
		hitCount[LogicalOperationTypeList::E_NotOr] != 0)
	{
		//OR��������ɂāA���莎�s�񐔂�1�ȏォ����񐔂�1�ȏ�ł���Ƃ��A
		//�������B�ɂāA���̃��[�v�������J�n(false)
		return false;
	}
	return true;
}

/**
* @fn void ColumnDataTypeChecker::execMatchForColumnType(int *commandCount, int *hitCount, std::string patternString, LogicalOperationTypeList logicalOperationType, std::wstring value)
* @brief Column�̃f�[�^���e�𐳋K�\���Ŕ�r�Ƙ_�����Z�q��񂩂琬�ۂ𔻒肷��֐�
* @param commandCount ���ߎ��s�񐔂̃J�E���^�[�p�z��
* @param hitCount ����񐔂̃J�E���^�[�p�z��
* @param patternString ����p���K�\�����`����������
* @param logicalOperationType ����p�_�����Z�p�����[�^
* @param value ���o�Ώە�����
* @details Column�̃f�[�^���e�𐳋K�\���Ŕ�r�Ƙ_�����Z�q��񂩂琬�ۂ𔻒肵�J�E���^�[�ϐ����J�E���g�A�b�v�����܂�
*/
void ColumnDataTypeChecker::execMatchForColumnType(int *commandCount, int *hitCount, std::string patternString, LogicalOperationTypeList logicalOperationType, std::wstring value)
{
	//�󂯎�������K�\���p������𐳋K�\���C���X�^���X�֕ϊ�
	wstring ws;
	stringToWString(ws, patternString);
	std::wregex pattern(ws);

	//�Ώۂ̃f�[�^�������Ƀ}�b�`���邩����
	bool flg = regex_match(value.c_str(), pattern);

	//���K�\���Ń}�b�`���邩�ǂ����𔻒肵���t���O��
	//�_�����Z�̔����g�ݍ��킹�������Ɉ�v�������̂�
	//���o�Ɣ��肳�ꂽ���̂Ƃ��񐔂��J�E���g����
	switch (logicalOperationType)
	{
	case LogicalOperationTypeList::E_And://AND��������
	case LogicalOperationTypeList::E_Or://OR��������
		commandCount[logicalOperationType]++;
		if (flg)
		{
			//������true���o���ꍇ������Z
			hitCount[logicalOperationType]++;
		}
		break;
	case LogicalOperationTypeList::E_NotAnd://NOT AND����
	case LogicalOperationTypeList::E_NotOr://NOT OR����
		commandCount[logicalOperationType]++;
		if (!flg)
		{
			//NOT������false���o���ꍇ������Z
			hitCount[logicalOperationType]++;
		}
		break;
	}
}

/**
* @fn bool ColumnDataTypeChecker::chackForDataTypeList(std::list<DataType> dataTypeList, std::wstring targetString)
* @brief �Ώۂ̃f�[�^�^�̌��o�Ώۂ��`�F�b�N���s���֐�
* @param dataTypeList ����p�f�[�^�^���X�g
* @param targetString �����Ώۃf�[�^�^
* @return ���o�Ώۂ����肵�����ʂ�bool�ŕԂ�
* @details �Ώۂ̃f�[�^�^�̌��o�Ώۂ��`�F�b�N���s���֐�
*/
bool ColumnDataTypeChecker::chackForDataTypeList(std::list<DataType> dataTypeList, std::wstring targetString)
{
	//�t���O������
	int hitCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };
	int exeCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };


	//�f�[�^�^���X�g�̐��������[�v����
	for (auto dataType = dataTypeList.begin(); dataType != dataTypeList.end(); dataType++)
	{
		//�Ώۂ̃f�[�^�^�̌��o�Ώۂ��`�F�b�N���s��
		this->execMatchForColumnType(exeCount,
			hitCount,
			(*dataType).dataType,
			(*dataType).logicalOperation,
			targetString);
	}

	//����m�F
	if (!this->checkForJudgment(exeCount, hitCount))
	{
		//���o�Ώۏo�Ȃ������ꍇ�Ăяo������false��Ԃ�
		return false;
	}
	return true;
}

/**
* @fn bool ColumnDataTypeChecker::chackFoTableOptionList(std::list<TableOption> tableOptions, std::list<std::wstring> DdlOptions)
* @brief �Ώۂ�Column�̃e�[�u���I�v�V�������������X�g�ƈ�v���邩�`�F�b�N���s���֐�
* @param tableOptions ����p�e�[�u���I�v�V�������X�g
* @param DdlOptions DDL�I�v�V����
* @return ���o�Ώۂ����肵�����ʂ�bool�ŕԂ�
* @details �Ώۂ�Column�̃e�[�u���I�v�V�������������X�g�ƈ�v���邩�`�F�b�N���s���֐�
*/
bool ColumnDataTypeChecker::chackFoTableOptionList(std::list<TableOption> tableOptions, std::list<std::wstring> DdlOptions)
{
	//�t���O������
	int tableOptionHitCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };
	int tableOptionExeCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };

	//DDL�I�v�V�����̐��������[�v����
	for (auto ddlOptionIt = DdlOptions.begin(); ddlOptionIt != DdlOptions.end(); ddlOptionIt++)
	{
		//�e�[�u���ɃZ�b�g���ꂽ�I�v�V�����̐��������[�v����
		for (auto tableOptionIt = tableOptions.begin(); tableOptionIt != tableOptions.end(); tableOptionIt++)
		{
			//�Ώۂ̃e�[�u���I�v�V���������o�Ώۂ̂��̂��`�F�b�N���s��
			this->execMatchForColumnType(tableOptionExeCount,
				tableOptionHitCount,
				TableOptionToLabel[(*tableOptionIt).tableOption],
				(*tableOptionIt).logicalOperation,
				(*ddlOptionIt));
		}
	}

	//����m�F
	if (!this->checkForJudgment(tableOptionExeCount, tableOptionHitCount))
	{
		//���o�Ώۏo�Ȃ������ꍇ�����A�Ăяo������false��Ԃ�
		return false;
	}
	return true;
}